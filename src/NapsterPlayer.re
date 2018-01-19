[@noserialize]
type initOptions = {.
    "consumerKey": string,
    "version": string,
    "catalog": Js.undefined(string),
    "player": Js.undefined(string)
};

[@bs.module "napster"] external _init : initOptions => unit = "init";

let _doInit = (catalog, player, consumerKey, version) => {
    _init({
        "consumerKey": consumerKey,
        "version": version,
        "catalog": Js.Undefined.from_opt(catalog),
        "player": Js.Undefined.from_opt(player)
    });

    None
};

let init = (~catalog=?, ~player=?, consumerKey, version) =>
    try (_doInit(catalog, player, consumerKey, version)) {
        | Js.Exn.Error(e) =>
            switch (Js.Exn.message(e)) {
                | Some(msg) => Some(msg)
                | None => Some("An unknown error occurred")
            }
    };

type player;
[@bs.module "napster"] external _player : player = "player";
[@bs.send] external _on : (player, string, 'a) => unit = "on";

type member;
[@bs.module "napster"] external _member : member = "member";

[@noserialize]
type setOptions = {.
    "accessToken": string,
    "refreshToken": string
};

[@bs.send] external _setAuth : (member, setOptions) => unit = "set";
[@bs.send] external _signedIn : member => Js.boolean = "signedIn";
[@bs.send] external _load : member => Js.boolean = "load";

let setAuth = (opts) => _setAuth(_member, opts);
let signedIn = () => _signedIn(_member);
let load = () => _load(_member);

[@noserialize]
type event('a) =
    | Error: event(Js.Json.t => unit)
    | Ready: event(unit => unit);

let on = (type t, event: event(t), handler: t) =>
    switch event {
        | Ready => _on(_player, "ready", handler)
        | Error => _on(_player, "error", handler)
    };
