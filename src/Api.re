type api;
type getResult = {.
  "error": Js.nullable(string),
  "status": int,
  "response": Js.t({.
      "code": string,
      "message": string
  })
};

[@bs.module "napster"] external api : api = "api";
[@bs.send] external _get : api => bool => string => (getResult => unit) => unit = "get";

let get = (secure, path, callback) => _get(api, secure, path, callback);