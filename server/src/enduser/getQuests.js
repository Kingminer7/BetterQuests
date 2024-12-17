import config from '../config';

const module = {
	name: 'EndUser:GetQuests',
	execute: async function (req, env, ctx) {
		if (req.url_.pathname !== '/enduser/getquests') return;
		if (isNaN(req.url_.searchParams.get('version'))) {
			return new Response(`{"error": {"code": "4", "reason": "Invalid version."}}`, {
				headers: { 'Content-Type': 'application/json' },
			});
		} else if (req.url_.searchParams.get('version') < 1) {
			return new Response(`{"error": {"code": "4", "reason": "Outdated version."}}`, {
				headers: { 'Content-Type': 'application/json' },
			})
		}


		var day = Math.floor(Math.floor(Date.now() / 1000) / (60 * 60 * 24));

		const resp = {
			day: day - config.startDate,
			next_reset: Math.floor(Date.now() / 1000 + 86400 - ((Date.now() / 1000) % 86400)),
			quests: [],
		};

		for (var type of ['easy', 'medium', 'hard', 'expert', 'extreme', 'master']) {
			var seed = day * env.secretnumberlol || 0;
			var hash = 0,
				len = type.length;
			for (var i = 0; i < len; i++) {
				hash = (hash << 5) - hash + type.charCodeAt(i);
				hash |= 0;
			}
			seed += hash;
			let query = `SELECT * FROM Quests WHERE Difficulty = '${type}' AND Enabled = 1 ORDER BY random() LIMIT 3;`;

			const { results } = await env.db.prepare(query).all();

			var res = await env.db.prepare(`SELECT MaximumTrophies, LastUpd${type}, LastRequestDate FROM GeneralData WHERE Id = 0`);
			res = await res.all();
			res = res.results[0];

			if (res[`LastRequestDate`] < day) {
				// get all quests in the QueuedQuests table and put them in the Quests table (ignore the ID value in QueuedQuests table)
				let ins = `INSERT INTO Quests (Name, Description, Specifications, Reward, Difficulty) SELECT Name, Description, Specifications, Reward, Difficulty FROM QueuedQuests;`;
				var rres = await env.db.prepare(ins).all();
				// delete all quests in the QueuedQuests table
				let del = `DELETE FROM QueuedQuests;`;
				var rres = await env.db.prepare(del).all();
				// update the LastRequestDate to today
				let ins2 = `UPDATE GeneralData SET LastRequestDate = ${day};`;
				var rres = await env.db.prepare(ins2).all();

				// get all levels in the QueuedLevels table and put them in the Levels table (ignore the ID value in QueuedLevels table)
				let ins3 = `INSERT INTO Levels (Name, LevelId, Difficulty) SELECT Name, LevelId, Difficulty FROM QueuedLevels;`;
				var rres = await env.db.prepare(ins3).all();
				// delete all levels in the QueuedLevels table
				let del2 = `DELETE FROM QueuedLevels;`;
				var rres = await env.db.prepare(del2).all();
				// update the LastRequestDate to today
				let ins4 = `UPDATE GeneralData SET LastRequestDate = ${day};`;
				var rres = await env.db.prepare(ins4).all();
			}

			if (res[`LastUpd${type}`] < day) {
				let i = res.MaximumTrophies;
				results.forEach((result) => {
					i += result.Reward;
				});
				let ins = `UPDATE GeneralData SET LastUpd${type} = ${day}, MaximumTrophies = ${i};`;
				var res = await env.db.prepare(ins).all();
			}
			let query2 = `SELECT * FROM Levels WHERE Difficulty = '${type}' ORDER BY random() LIMIT 3;`;
			const levels = await env.db.prepare(query2).all();
			for (var i = 0; i < results.length; i++) {
				if (
					results[i].Name.includes(`{{levelname}}`) ||
					results[i].Description.includes(`{{levelname}}`) ||
					results[i].Specifications.includes(`{{levelname}}`)
				) {
					const level = levels.results[i] || {
						Name: 'No Level',
						LevelId: 0,
						QuestName: '',
					}
					results[i].Name = results[i].Name.replace(`{{levelname}}`, level.Name).replace(
						`{{levelid}}`,
						level.LevelId
					);
					results[i].Description = results[i].Description.replace(`{{levelname}}`, level.Name).replace(
						`{{levelid}}`,
						level.LevelId
					);
					results[i].Specifications = results[i].Specifications.replace(`{{levelname}}`, level.Name).replace(
						`{{levelid}}`,
						level.LevelId
					);
					if (level.QuestName !== '') results[i].Name = level.QuestName;
				}
			}
			// console.log(results)
			results.forEach(res=>{
				resp.quests.push(res)
			})
		};
		return new Response(JSON.stringify(resp), {
			headers: { 'Content-Type': 'application/json' },
		});
		// return new Response(`{"day": ${day - config.startDate}, "quests": ${JSON.stringify(results)}, "next_reset": ${Math.floor(Date.now()/1000 + 86400 - (Date.now() / 1000) % 86400)}}`, {
		// 	headers: { 'Content-Type': 'application/json' },
		// });
	},
};

export default module;
