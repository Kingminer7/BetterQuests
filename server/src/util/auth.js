function generateUUID() {
	let dt = new Date().getTime();
	const uuid = 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
		const r = (dt + Math.random() * 16) % 16 | 0;
		dt = Math.floor(dt / 16);
		return (c === 'x' ? r : (r & 0x3) | 0x8).toString(16);
	});
	return uuid;
}

import config from '../config';

async function request(req, env, ctx) {
	var json;
	try {
		json = await (await req.clone()).json();
	} catch (e) {
		return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});
	}

	const id = parseInt(json.id);
	if (!id)
		return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});

	var challenge = (await env.db.prepare(`SELECT * FROM AuthRequests WHERE UserId = ${parseInt(id)};`).all()).results[0];
	if (challenge != null) {
		if (parseInt(challenge.Expires) < Math.floor(Date.now() / 1000)) {
			challenge = null;
			await env.db.prepare(`DELETE FROM AuthRequests WHERE UserId = ${parseInt(id)};`).all();
		}
	}

	if (challenge == null) {
		const expires = Math.floor(Date.now() / 1000) + config.authReqExp;
		const code = Math.random().toString(36);
		challenge = { UserId: id, Challenge: code, Expires: expires };
		var r = await env.db.prepare(`INSERT INTO AuthRequests (UserId, Challenge, Expires) VALUES (${id}, '${code}', ${expires});`).all();
	}

	return new Response(`{"challenge": "${challenge.Challenge}"}`, {
		headers: { 'Content-Type': 'application/json' },
	});
}

async function verify(req, env, ctx) {
	var json;
	try {
		json = await (await req.clone()).json();
	} catch (e) {
		return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});
	}

	if (!json.id)
		return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});

	if (!json.messageid)
		return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});

	var challenge = (await env.db.prepare(`SELECT * FROM AuthRequests WHERE UserId = ${parseInt(json.id)};`).all()).results[0];
	if (challenge != null) {
		if (parseInt(challenge.Expires) < Math.floor(Date.now() / 1000)) {
			challenge = null;
			await env.db.prepare(`DELETE FROM AuthRequests WHERE UserId = ${parseInt(json.id)};`).all();
		}
	}

	if (challenge == null) {
		return new Response(`{"error": {"code": "3", "reason": "Invalid challenge."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});
	}

	const headers = {
		'Content-Type': 'application/x-www-form-urlencoded',
		'User-Agent': '',
	};

	const resp = await fetch(`${config.gdServers}/downloadGJMessage20.php`, {
		method: 'POST',
		headers: headers,
		body: 'accountID=' + env.accountid + '&gjp2=' + env.gjp2 + '&messageID=' + json.messageid + '&secret=Wmfd2893gb7',
	}).then((r) => r.text());

	const respArr = resp.split(':');
	const respObj = {};
	for (let i = 0; i < respArr.length; i += 2) {
		respObj[respArr[i]] = respArr[i + 1];
	}

	console.log(atob(respObj['4']));
	if (atob(respObj['4']) != challenge.Challenge) {
		return new Response(`{"error": {"code": "1", "reason": "Incorrect challenge."}}`, {
			headers: { 'Content-Type': 'application/json' },
		});
	}

	const expires = Math.floor(Date.now() / 1000) + config.authTokenExp;
	var token = generateUUID();
	var r = (await env.db.prepare(`SELECT * FROM Auth WHERE UserId = ${parseInt(json.id)};`).all()).results[0];
	if (r) {
		if (r.Expires < Math.floor(Date.now() / 1000)) {
			r = await env.db.prepare(`UPDATE Auth SET Token = '${token}', Expires = ${expires} WHERE UserId = ${parseInt(json.id)};`).all();
			token = r.Token;
		} else {
			token = r.Token;
		}
	} else {
		r = await env.db.prepare(`INSERT INTO Auth (UserId, Token, Expires) VALUES (${json.id}, '${token}', ${expires});`).all();
		token = r.Token;
	}
	await env.db.prepare(`DELETE FROM AuthRequests WHERE UserId = ${parseInt(json.id)};`).all();

	return new Response(`{"token": "${token}"}`, {
		headers: { 'Content-Type': 'application/json' },
	});
}

const module = {
	name: 'auth',
	async execute(req, env, ctx) {
		if (req.url_.pathname == '/auth/request') return await request(req, env, ctx);
		if (req.url_.pathname == '/auth/verify') return await verify(req, env, ctx);
	},
};

export default module;

export async function checkAuth(accId, token, env) {
	const r = await env.db.prepare(`SELECT * FROM Auth WHERE UserId = ${accId} AND Token = '${token}';`).all();
	if (r.results.length == 0)
		return {
			resp: new Response(`{"error": {"code": "1", "reason": "Invalid account authentication."}}`, {
				headers: { 'Content-Type': 'application/json' },
			}),
		};
	if (r.results[0].Expires < Math.floor(Date.now() / 1000)) {
		await env.db.prepare(`DELETE FROM Auth WHERE UserId = ${accId};`).all();
		return {
			resp: new Response(`{"error": {"code": "1", "reason": "Invalid account authentication."}}`, {
				headers: { 'Content-Type': 'application/json' },
			}),
		};
	}

	let query = `SELECT * FROM Users WHERE GdId = '${r.UserId}' LIMIT 1`;
	var { results } = await db.prepare(query).all();
	if (results.length == 0) {
		if (!config.openForSignups)
			return {
				resp: new Response(`{"error": {"code": "1", "reason": "Not currently accepting signups."}}`, {
					headers: { 'Content-Type': 'application/json' },
				}),
			};
		let ins = `INSERT INTO Users (GdId, Name) VALUES ('${r.UserId}', 'Unknown');`;
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
