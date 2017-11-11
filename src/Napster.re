module Api = {
    include Api;
};

type initOptions = Js.t {.
    consumerKey: string,
    version: string,
    catalog: Js.undefined string,
    player: Js.undefined string
} [@@noserialize];

external _init : initOptions => unit = "init" [@@bs.module "napster"];

let _doInit catalog player consumerKey version => {
    _init {
        "consumerKey": consumerKey,
        "version": version,
        "catalog": Js.Undefined.from_opt catalog,
        "player": Js.Undefined.from_opt player
    };

    None;
};

let init ::catalog=? ::player=? consumerKey version => {
    try (_doInit catalog player consumerKey version) {
        | Js.Exn.Error e => {
            switch (Js.Exn.message e) {
                | Some msg => Some msg
                | None => Some "An unknown error occurred"
            };
        }
    };
};

type player;
external _player : player = "player" [@@bs.module "napster"];
external _on : player => string => 'a => unit = "on" [@@bs.send];

type member;
external _member : member = "member" [@@bs.module "napster"];

type setOptions = Js.t {.
    accessToken: string,
    refreshToken: string
} [@@noserialize];

external _setAuth : member => setOptions => unit = "set" [@@bs.send];
external _signedIn : member => Js.boolean = "signedIn" [@@bs.send];
external _load : member => Js.boolean = "load" [@@bs.send];

let setAuth opts => _setAuth _member opts;
let signedIn () => _signedIn _member;
let load () => _load _member;

type event 'a =
    | Error : event (Js.Json.t => unit)
    | Ready : event (unit => unit) [@@noserialize];

let on (type t) (event : event t) (handler : t) => {
    switch event {
        | Ready => _on _player "ready" handler
        | Error => _on _player "error" handler
    };
};