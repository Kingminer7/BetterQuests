import config from "../config";

/*
# OLD

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

        let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' ORDER BY SIN(sqlid + ${seed}) LIMIT 3`;

        const { results } = await env.db.prepare(query).all();

        var res = await env.db.prepare(`SELECT MaximumTrophies, LastUpd${type} FROM GeneralData WHERE Id = 0`);
        res = await res.all();
        res = res.results[0];

        if (res[`LastUpd${type}`] < day) {
          let i = res.MaximumTrophies;
          results.forEach(result => {
            i += result.Reward
          });
          let ins = `UPDATE GeneralData SET LastUpd${type} = ${day}, MaximumTrophies = ${i};`
          var res = await env.db.prepare(ins).all();
        }
        return new Response(`{"day": ${day - config.startDate}, "quests": ${JSON.stringify(results)}}`, {
          headers: { 'Content-Type': 'application/json' }
        });
      }

*/

const module = {
	name: 'EndUser:GetQuests',
	execute: async function (req, env, ctx) {

		if (req.url_.pathname !== '/enduser/getquests') return;
		const type = (req.url_.searchParams.get('type') || "").toLowerCase();
		if(![
			"easy",
			"medium",
			"hard",
			"expert",
			"master",
			"grandmaster"
		].includes(type)) return new Response(`{"error": {"code": "2", "reason": "Invalid difficulty."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});


		var day = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
        var seed = day * env.secretnumberlol
        var hash = 0, len = type.length;
        for (var i = 0; i < len; i++) {
          hash = ((hash << 5) - hash) + type.charCodeAt(i);
          hash |= 0;
        }
        seed += hash;

        let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' ORDER BY SIN(${seed} + Id) LIMIT 3`;

        const { results } = await env.db.prepare(query).all();

        var res = await env.db.prepare(`SELECT MaximumTrophies, LastUpd${type} FROM GeneralData WHERE Id = 0`);
        res = await res.all();
        res = res.results[0];

		console.log(res);


        if (res[`LastUpd${type}`] < day) {
          let i = res.MaximumTrophies;
          results.forEach(result => {
            i += result.Reward
          });
          let ins = `UPDATE GeneralData SET LastUpd${type} = ${day}, MaximumTrophies = ${i};`
          var res = await env.db.prepare(ins).all();
        }
        return new Response(`{"day": ${day - config.startDate}, "quests": ${JSON.stringify(results)}}`, {
          headers: { 'Content-Type': 'application/json' }
        });
	},
};

export default module;
