type api;
type getResult = Js.t ({.
  "error": string,
  "status": int,
  "response": Js.t({.
      "code": string,
      "message": string
  })
});

[@bs.module "napster"] external api : api = "api";
[@bs.send] external _get : api => Js.boolean => string => (getResult => unit) => unit = "get";

let get = (secure, path, callback) => _get(api, secure, path, callback);