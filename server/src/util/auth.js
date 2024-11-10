import config from '../config';

async function checkAuth(token) {
  const params = new URLSearchParams();
  params.append('sessionID', token);
  return (
    await fetch('https://gd.figm.io/authentication/validate', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/x-www-form-urlencoded',
      },
      body: params.toString(),
    })
  ).json();
}

async function login(db, token) {
  const res = await this.checkAuth(token);
  if (res == '-1')
    return {
      resp: new Response(`{"error": {"code": "1", "reason": "Invalid account authentication."}}`, {
        headers: { 'Content-Type': 'application/json' },
      }),
    };
  let query = `SELECT * FROM Users WHERE GdId = '${res.accountID}' LIMIT 1`;
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
