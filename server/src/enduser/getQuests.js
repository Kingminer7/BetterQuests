import config from '../config';

const module = {
	name: 'EndUser:GetQuests',
	execute: async function (req, env, ctx) {
		if (req.url_.pathname !== '/enduser/getquests') return;
		const type = (req.url_.searchParams.get('type') || '').toLowerCase();
		if (!['easy', 'medium', 'hard', 'expert', 'master', 'grandmaster'].includes(type))
			return new Response(`{"error": {"code": "2", "reason": "Invalid difficulty."}}`, {
				headers: { 'Content-Type': 'application/json' },
			});

		var day = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));
		var seed = day * env.secretnumberlol;
		var hash = 0,
			len = type.length;
		for (var i = 0; i < len; i++) {
			hash = (hash << 5) - hash + type.charCodeAt(i);
			hash |= 0;
		}
		seed += hash;

		let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' ORDER BY SIN(${seed} + Id) LIMIT 3`;

		const { results } = await env.db.prepare(query).all();

		var res = await env.db.prepare(`SELECT MaximumTrophies, LastUpd${type} FROM GeneralData WHERE Id = 0`);
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
		for (var i = 0; i < results.length; i++) {
			// pick level like picked quests
			let level = await env.db.prepare(`SELECT * FROM Levels WHERE Difficulty = '${type}' ORDER BY SIN(${seed} + Id) LIMIT 1`);
			level = await level.all();
			if (level.results.length === 0) {
				return new Response(`{"error": {"code": "3", "reason": "No levels found."}}`, {
					headers: { 'Content-Type': 'application/json' },
				});
			}
			results[i].Name = results[i].Name.replace(`{{levelname}}`, level.results[0].Name).replace(`{{levelid}}`, level.results[0].LevelId);
			results[i].Description = results[i].Description.replace(`{{levelname}}`, level.results[0].Name).replace(`{{levelid}}`, level.results[0].LevelId);
			results[i].Specifications = results[i].Specifications.replace(`{{levelname}}`, level.results[0].Name).replace(`{{levelid}}`, level.results[0].LevelId);
		}
		return new Response(`{"day": ${day - config.startDate}, "quests": ${JSON.stringify(results)}}`, {
			headers: { 'Content-Type': 'application/json' },
		});
	},
};

export default module;
