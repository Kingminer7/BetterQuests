const module = {
  name: 'Generic:404',
  execute: function (request, env, ctx) {
    return new Response("404 Not Found", {
      status: 404,
      statusText: "Not Found"
    })
  },
};

module.exports = module