async function checkAuth(token) {
  const params = new URLSearchParams();
  params.append('sessionID', token);
  return (await fetch("https://gd.figm.io/authentication/validate", {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
    },
    body: params.toString()
  })).json();
}

const config = {
  openForSignups: false,

}

export default {
  async fetch(request, env) {
    try {
      const url = new URL(request.url);

      const pathSegments = url.pathname.split('/').filter(segment => segment !== '');

      if (url.pathname === "/api/all") {
        const type = pathSegments.slice(2)[0];
        var seed = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
        seed *= env.secretnumberlol
        var hash = 0, len = 3;
        for (var i = 0; i < len; i++) {
          hash = ((hash << 5) - hash) + "all".charCodeAt(i);
          hash |= 0;
        }
        seed += hash;

        let query = `SELECT * FROM Quests ORDER BY SIN(sqlid + ${seed}) LIMIT 3`;

        const { results } = await env.db.prepare(query).all();
        return new Response(JSON.stringify(results), {
          headers: { 'Content-Type': 'application/json' }
        });
      }

      // Getting daily quests

      if (pathSegments.length == 3 && pathSegments[0] === 'api' && pathSegments[1] === 'daily') {
        const type = pathSegments.slice(2)[0];
        var day = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
        var seed = day * env.secretnumberlol
        var hash = 0, len = type.length;
        for (var i = 0; i < len; i++) {
          hash = ((hash << 5) - hash) + type.charCodeAt(i);
          hash |= 0;
        }
        seed += hash;

        let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' ORDER BY SIN(sqlid + ${seed})`;// LIMIT 3`;

        const { results } = await env.db.prepare(query).all();
        return new Response(`{"day": ${day - 20032}, "quests": ${JSON.stringify(results)}}`, {
          headers: { 'Content-Type': 'application/json' }
        });
      }

      let s = "{";

      url.searchParams.forEach((value, key) => {
        s += `"${key}":"${value}"`
      })
      s += "}"
      if (url.pathname === "/api/admin/test") {
        let r2 = await request.clone();
        var json;
        try {
          json = await r2.json();
        } catch (e) {
          return new Response("400 Bad Request", {
            status: 400,
            statusText: "Bad Request",
          });
        }
        // return new Response(await checkAuth(json.token), {
        //   headers: {"Content-Type": "text/plain"}
        // });
        const res = await checkAuth(json.token);
        if (res == "-1")
          return new Response("401 Unauthorized", {
            status: 401,
            statusText: "Unauthorized",
          });
        let query = `SELECT * FROM Users WHERE GdId = '${res.accountID}' LIMIT 1`;
        var { results } = await env.db.prepare(query).all();
        if (results.length == 0) {
          if (!config.openForSignups) return new Response("403 Forbidden", {
            status: 403,
            statusText: "Forbidden",
          })
          let ins = `INSERT INTO Users (GdId, Username) VALUES ('${res.accountID}', '${res.username}');`
          await env.db.prepare(ins).all();
          results = (await env.db.prepare(query).all()).results;
        }

        if(results.length == 0) {
          return new Response("500 Internal Server Error", {
            status: 500,
            statusText: "Internal Server Error",
          })
        }

        return new Response(results[0].Administrator, {
          headers: { "Content-Type": "text/plain" }
         });
      }

      // TODO: admin crap

      // TODO: leaderboard

      return new Response("404 Not Found", {
        status: 404,
        statusText: "Not Found",
      });
    } catch (e) {
      return new Response(`Internal error occured! ${e}`, {
        status: 500,
        statusText: "Not Found",
      });
    }
  },
};
