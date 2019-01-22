type api;
type getResult = {.
    "error": Js.nullable(string),
    "response": Js.t({.
        "code": string,
        "message": string
    }),
    "status": int
};
let api: api;
let get: (bool, string, getResult => unit) => unit;
