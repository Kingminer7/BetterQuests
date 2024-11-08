const module = {
	name: 'Generic:404',
	execute: async function (request, env, ctx) {
		return new Response('404 Not Found', {
			status: 404,
			statusText: 'Not Found',
		});
	},
};

export default module;
