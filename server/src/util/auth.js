import config from '../config';

async function checkAuth(token) {
	return await fetch(config.authServer, {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json',
		},
		body: {
			token: token,
		},
	});
}

async function login(db, token) {
	// check db Auth table
	let query = `SELECT * FROM Auth WHERE Token = '${token}' LIMIT 1`;
	var { results } = await db.prepare(query).all();
	if (results.length == 1) {
		if (results[0].Expires < Date.now()) {
			let del = `DELETE FROM Auth WHERE Token = '${token}'`;
			await db.prepare(del).all();
			return {
				resp: new Response(`{"error": {"code": "1", "reason": "Token expired."}}`, {
					headers: { 'Content-Type': 'application/json' },
				}),
			};
		}
	} else {
		const res = await this.checkAuth(token);
		if (!res.ok)
			return {
				resp: new Response(`{"error": {"code": "1", "reason": "Invalid account authentication."}}`, {
					headers: { 'Content-Type': 'application/json' },
				}),
			};
	}

	query = `SELECT * FROM Users WHERE GdId = '${res.accountID}' LIMIT 1`;
	var { results } = await db.prepare(query).all();
	if (results.length == 0) {
		if (!config.openForSignups)
			return {
				resp: new Response(`{"error": {"code": "1", "reason": "Not currently accepting signups."}}`, {
					headers: { 'Content-Type': 'application/json' },
				}),
			};
		let ins = `INSERT INTO Users (GdId, Name) VALUES ('${res.accountID}', '${res.username}');`;
		await db.prepare(ins).all();
		results = (await db.prepare(query).all()).results;
	}

	if (results.length == 0) {
		return {
			resp: new Response('500 Internal Server Error', {
				status: 500,
				statusText: 'Internal Server Error',
			}),
		};
	}

	return {
		user: results[0],
	};
}

export default {
	checkAuth,
	login,
};
