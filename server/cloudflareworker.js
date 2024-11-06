export default {
  async fetch(request, env) {
    const url = new URL(request.url);

    const pathSegments = url.pathname.split('/').filter(segment => segment !== '');

    if (url.pathname === "/api/all") {
      const type = pathSegments.slice(2)[0];
      var seed = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
      seed *= env.secretnumberlol
      var hash = 0, len = 3;
      for (var i = 0; i < len; i++) {
        hash  = ((hash << 5) - hash) + "all".charCodeAt(i);
        hash |= 0;
      }
      seed += hash;
      
      let query = `SELECT * FROM Quests ORDER BY SIN(sqlid + ${seed})`;// LIMIT 3`;

      const { results } = await env.db.prepare(query).all();
      return new Response(JSON.stringify(results), {
        headers: { 'Content-Type': 'application/json' }
      });
    }

    if (pathSegments.length == 3 && pathSegments[0] === 'api' && pathSegments[1] === 'daily') {
      const type = pathSegments.slice(2)[0];
      var seed = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
      seed *= env.secretnumberlol
      var hash = 0, len = type.length;
      for (var i = 0; i < len; i++) {
        hash  = ((hash << 5) - hash) + type.charCodeAt(i);
        hash |= 0;
      }
      seed += hash;
      
      let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' ORDER BY SIN(sqlid + ${seed})`;// LIMIT 3`;

      const { results } = await env.db.prepare(query).all();
      return new Response(JSON.stringify(results), {
        headers: { 'Content-Type': 'application/json' }
      });
    }

    // TODO: remove session tokens after expiration

    // TODO: admin crap

    // TODO: leaderboard

    return new Response("404 Not Found", {
      status: 404,
      statusText: "Not Found",
    });
  },
};
