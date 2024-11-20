import config from '../config';

const module = {
  name: "auth",
  async execute(req, env, ctx) {
    if (req.url_.pathname !== '/auth/request') return;
    var json;
    try {
      json = await (await req.clone()).json();
    } catch (e) {
      return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
        headers: { 'Content-Type': 'application/json' },
      });
    }

    const id = json.id
    if (!id)
      return new Response(`{"error": {"code": "2", "reason": "Invalid request."}}`, {
        headers: { 'Content-Type': 'application/json' },
      });

    var challenge = (await env.db.prepare(`SELECT * FROM Auth WHERE UserId = ${parseInt(id)};`).all()).results[0];
    if (challenge != null) {
      if (parseInt(challenge.Expires) < Date.now()) {
        challenge = null;
        env.db.prepare(`DELETE FROM Challenges WHERE Id = ${parseInt(id)};`).all();
      }
    }
  }
}

export default module
