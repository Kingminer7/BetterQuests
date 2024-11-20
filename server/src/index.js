// TODO: Prevent SQL Injection

import addQuest from './admin/addQuest';
import getQuests from './enduser/getQuests';
import addLevel from './admin/addLevel';
import auth from './util/auth';
let modules = [addQuest, addLevel, getQuests, auth];

export default {
	async fetch(req, env, ctx) {
		req.url_ = new URL(req.url);
		for (let module of modules) {
			var res = await module.execute(req, env, ctx);
			if (res) return res;
		}
		return new Response('404 Not Found', {
			status: 404,
			statusText: 'Not Found',
		});
	},
};
