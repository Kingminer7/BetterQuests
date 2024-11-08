import addQuest from "./admin/addQuest";
import _404 from "./admin/addQuest";
let modules = [addQuest]

export default {
	async fetch(request, env, ctx) {
    var res = addQuest.execute(req, env, ctx)
		if (res) return res;
    res = _404.execute(req, env, ctx)
		if (res) return res;
	},
};
