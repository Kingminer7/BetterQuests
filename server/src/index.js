import addQuest from './admin/addQuest';
let modules = [addQuest];

export default {
	async fetch(req, env, ctx) {
    req.url_ = new URL(req.url);
    var res = await addQuest.execute(req, env, ctx);
		if (res) return res;
		var res = await testRubRub.execute(req, env, ctx);
		if (res) return res;
		return new Response('404 Not Found', {
			status: 404,
			statusText: 'Not Found',
		});
	},
};
