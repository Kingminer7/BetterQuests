import addQuest from './admin/addQuest';
import _404 from './generic/404';
let modules = [addQuest];

export default {
	async fetch(req, env, ctx) {
    req.url_ = new URL(req.url);
		var res = await addQuest.execute(req, env, ctx);
		if (res) return res;
		res = await _404.execute(req, env, ctx);
		if (res) return res;
	},
};
