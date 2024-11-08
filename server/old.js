async function checkAuth(token) {
  const params = new URLSearchParams();
  params.append("sessionID", token);
  return (
    await fetch("https://gd.figm.io/authentication/validate", {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
      },
      body: params.toString(),
    })
  ).json();
}

async function login(db, token) {
  const res = await checkAuth(token);
  if (res == "-1")
    return {
      fail: true,
      resp: new Response(
        `{"error": {"code": "1", "reason": "Invalid account authentication."}}`,
        {
          headers: { "Content-Type": "application/json" },
        }
      ),
    };
  let query = `SELECT * FROM Users WHERE GdId = '${res.accountID}' LIMIT 1`;
  var { results } = await db.prepare(query).all();
  if (results.length == 0) {
    if (!config.openForSignups)
      return {
        fail: true,
        resp: new Response(
          `{"error": {"code": "1", "reason": "Not currently accepting signups."}}`,
          {
            headers: { "Content-Type": "application/json" },
          }
        ),
      };
    let ins = `INSERT INTO Users (GdId, Username) VALUES ('${res.accountID}', '${res.username}');`;
    await db.prepare(ins).all();
    results = (await db.prepare(query).all()).results;
  }

  if (results.length == 0) {
    return {
      fail: true,
      resp: new Response("500 Internal Server Error", {
        status: 500,
        statusText: "Internal Server Error",
      }),
    };
  }

  return {
    fail: false,
    user: results[0],
  };
}

const config = {
  openForSignups: false,
  pageSize: 10,
  startDate: 20034,
};

export default {
  async fetch(request, env) {
    try {
      const url = new URL(request.url);

      const pathSegments = url.pathname
        .split("/")
        .filter((segment) => segment !== "");

      // Quest getting endpoints

      if (
        pathSegments.length == 3 &&
        pathSegments[0] === "api" &&
        pathSegments[1] === "daily"
      ) {
        const type = pathSegments.slice(2)[0];
        var day = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
        var seed = day * env.secretnumberlol;
        var hash = 0,
          len = type.length;
        for (var i = 0; i < len; i++) {
          hash = (hash << 5) - hash + type.charCodeAt(i);
          hash |= 0;
        }
        seed += hash;

        let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' ORDER BY SIN(sqlid + ${seed}) LIMIT 3`;

        const { results } = await env.db.prepare(query).all();

        var res = await env.db.prepare(
          `SELECT MaximumTrophies, LastUpd${type} FROM GeneralData WHERE Id = 0`
        );
        res = await res.all();
        res = res.results[0];

        if (res[`LastUpd${type}`] < day) {
          let i = res.MaximumTrophies;
          results.forEach((result) => {
            i += result.Reward;
          });
          let ins = `UPDATE GeneralData SET LastUpd${type} = ${day}, MaximumTrophies = ${i};`;
          var res = await env.db.prepare(ins).all();
        }
        return new Response(
          `{"day": ${day - config.startDate}, "quests": ${JSON.stringify(
            results
          )}}`,
          {
            headers: { "Content-Type": "application/json" },
          }
        );
      }

      // Administrator Endpoints

      if (url.pathname === "/api/admin/addquest") {
        let r2 = await request.clone();
        var json;
        try {
          json = await r2.json();
        } catch (e) {
          return new Response(
            `{"error": {"code": "2", "reason": "Incorrect request type."}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );
        }

        var { fail, user, resp } = await login(env.db, json.token);

        if (fail) return resp;

        if (user.Administrator <= 0) {
          return new Response(
            `{"error": {"code": "1", "reason": "Incorrect administration level."}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );
        }

        var missing = [];
        if (json.name == null) missing.push("name");
        if (json.diff == null) missing.push("diff");
        if (json.reward == null) missing.push("reward");
        if (json.type == null) missing.push("type");
        if (json.specs == null) missing.push("specs");

        if (missing.length > 0)
          return new Response(
            `{"error": "Missing components!","components": ${JSON.stringify(
              missing
            )}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );

        let ins = `INSERT INTO Quests (QuestName, Difficulty, Reward, QuestType, Specifiers) VALUES ('${json.name}', '${json.diff}', '${json.reward}', '${json.type}', '${json.specs}');`;
        var res = await env.db.prepare(ins).all();
        return new Response(
          `{"success": "${res.success}", "id": "${res.meta.last_row_id}"}`,
          {
            headers: { "Content-Type": "application/json" },
          }
        );
      }

      if (url.pathname === "/api/admin/addlevel") {
        let r2 = await request.clone();
        var json;
        try {
          json = await r2.json();
        } catch (e) {
          return new Response(
            `{"error": {"code": "2", "reason": "Incorrect request type."}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );
        }

        var { fail, user, resp } = await login(env.db, json.token);

        if (fail) return resp;

        if (user.Administrator <= 0) {
          return new Response(
            `{"error": {"code": "1", "reason": "Incorrect administration level."}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );
        }

        var missing = [];
        if (json.name == null) missing.push("name");
        if (json.id == null) missing.push("id");
        if (json.diff == null) missing.push("diff");

        if (missing.length > 0)
          return new Response(
            `{"error": "Missing components!","components": ${JSON.stringify(
              missing
            )}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );

        let ins = `INSERT INTO Levels (LevelName, LevelId, Difficulty) VALUES ('${json.name}', '${json.id}', '${json.diff}');`;
        var res = await env.db.prepare(ins).all();
        return new Response(
          `{"success": "${res.success}", "id": "${res.meta.last_row_id}"}`,
          {
            headers: { "Content-Type": "application/json" },
          }
        );
      }

      // Leaderboard endpoints
      if (url.pathname == "/api/leaderboard/get") {
        let page = parseInt(url.searchParams.get("page")) || 1;
        const totalCountQuery = `
          SELECT COUNT(*) AS total_count 
          FROM Users 
          WHERE Banned = 0
        `;

        const totalCount = (await env.db.prepare(totalCountQuery).all())
          .results[0].total_count;

        const totalPages = Math.ceil(totalCount / config.pageSize);

        if (page > totalPages || 0 >= page)
          return new Response(`{"total_pages": ${totalPages}}`, {
            headers: { "Content-Type": "application/json" },
          });
        let query = `SELECT * FROM Users WHERE Banned = 0 ORDER BY Trophies LIMIT 10`;
        const { results } = await env.db.prepare(query).all();

        let sentList = [];
        results.forEach((result) => {
          sentList.push({
            id: result.GdId,
            name: result.Username,
            trophies: result.Trophies,
          });
        });

        return new Response(
          `{"total_pages": ${totalPages}, "list": ${JSON.stringify(sentList)}}`,
          {
            headers: { "Content-Type": "application/json" },
          }
        );
      }

      if (url.pathname == "/api/leaderboard/update") {
        let r2 = await request.clone();
        var json;
        try {
          json = await r2.json();
        } catch (e) {
          return new Response(
            `{"error": {"code": "2", "reason": "Incorrect request level."}}`,
            {
              headers: { "Content-Type": "application/json" },
            }
          );
        }

        var { fail, user, resp } = await login(env.db, json.token);

        if (fail) return resp;

        let page = parseInt(url.searchParams.get("page")) || 1;
        const totalCountQuery = `
          SELECT COUNT(*) AS total_count 
          FROM Users 
          WHERE Banned = 0
        `;

        const totalCount = (await env.db.prepare(totalCountQuery).all())
          .results[0].total_count;

        const totalPages = Math.ceil(totalCount / config.pageSize);

        if (page > totalPages || 0 >= page)
          return new Response(`{"total_pages": ${totalPages}}`, {
            headers: { "Content-Type": "application/json" },
          });
        let query = `SELECT * FROM Users WHERE Banned = 0 ORDER BY Trophies LIMIT 10`;
        const { results } = await env.db.prepare(query).all();

        let sentList = [];
        results.forEach((result) => {
          sentList.push({
            id: result.GdId,
            name: result.Username,
            trophies: result.Trophies,
          });
        });

        return new Response(
          `{"total_pages": ${totalPages}, "list": ${JSON.stringify(sentList)}}`,
          {
            headers: { "Content-Type": "application/json" },
          }
        );
      }

      // TODO: Removing quests/levels
      // TODO: Proper banning support
      // TODO: Leaderboard

      return new Response("404 Not Found", {
        status: 404,
        statusText: "Not Found",
      });
    } catch (e) {
      return new Response(`500 Internal Server Error: ${e}`, {
        status: 500,
        statusText: "Internal Server Error",
      });
    }
  },
};
