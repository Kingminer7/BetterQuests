import auth from '../util/auth_old';

const module = {
	name: 'Admin:AddLevel',
	config: {
		minPerm: 1,
	},
	execute: async function (request, env, ctx) {
		if (request.url_.pathname !== '/admin/addlevel') return;
		var json;
		try {
			json = await (await request.clone()).json();
		} catch (e) {
			return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
				headers: { 'Content-Type': 'application/json' },
			});
		}

		var { user, resp } = await auth.login(env.db, json.token);
		if (resp) return resp;

		if (user.Administrator <= this.config.minPerm) {
			return new Response(`{"error": {"code": "1", "reason": "Unauthorized."}}`, {
				headers: { 'Content-Type': 'application/json' },
			});
		}

		var missing = [];
		if (json.name == null) missing.push('name');
		if (json.diff == null) missing.push('diff');
		if (json.id == null) missing.push('id');

		if (missing.length > 0) {
			return new Response(`{"error": {"code": "2", "reason": "Missing ${missing.join(', ')}."}}`, {
				headers: { 'Content-Type': 'application/json' },
			});
		}

		let ins = `INSERT INTO QueuedLevels (Name, LevelId, Difficulty) VALUES ('${json.name}', '${json.id}', '${json.diff}');`;
		var res = await env.db.prepare(ins).all();
		return new Response(`{"id": "${res.meta.last_row_id}"}`, {
			headers: { 'Content-Type': 'application/json' },
		});
	},
};

export default module;
