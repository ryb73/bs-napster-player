type api;
type getResult = {.
    "error": Js.nullable(string),
    "status": int,
    "response": Js.t({.
        "code": string,
        "message": string
    })
};

[@bs.module "@ryb73/napster"] external api : api = "api";
[@bs.send] external get : api => bool => string => (getResult => unit) => unit = "get";

let get = (secure, path, callback) => get(api, secure, path, callback);