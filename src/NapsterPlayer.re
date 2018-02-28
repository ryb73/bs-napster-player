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
        | Js.Exn.Error(e) => Some(e)
    };

type player;
[@bs.module "napster"] external _player : player = "player";
[@bs.send] external _on : (player, string, _ => unit) => unit = "on";

type member;
[@bs.module "napster"] external _member : member = "member";

[@noserialize]
type setOptions = {.
    "accessToken": string,
    "refreshToken": string
};

[@bs.send] external _setAuth : (member, setOptions) => unit = "set";
[@bs.send] external _auth : player => unit = "auth";
[@bs.send] external _signedIn : member => Js.boolean = "signedIn";
[@bs.send] external _load : member => unit = "load";

let setAuth = (opts) => _setAuth(_member, opts);
let auth = () => _auth(_player);
let tokensSet = () => _signedIn(_member) |> Js.to_bool;
let load = () => _load(_member);

let onReady = (handler) => _on(_player, "ready", () => handler(_player));
let onPlayStopped = (handler: unit => unit) => _on(_player, "playstopped", handler);

/* TODO: from_json ppx  */
type playEventCode =
    | PlayStarted
    | BufferFull
    | PlayComplete
    | Connected;

type playEvent = {
    code: playEventCode,
    id: string,
    paused: bool,
    playing: bool
};
let onPlayEvent = (handler) => {
    _on(_player, "playevent", (json) => handler({
        code: switch (json##data##code) {
            | "PlayComplete" => PlayComplete
            | "Connected" => Connected
            | _ => failwith("Unrecognized event: " ++ json##data##code)
        },
        id: json##data##id,
        paused: Js.to_bool(json##data##paused),
        playing: Js.to_bool(json##data##playing)
    }));
};

[@autoserialize]
type playTimerEvent = {
    currentTime: float,
    totalTime: float
};
let onPlayTimer = (handler) =>
    _on(_player, "playtimer", (json) => {
        switch (playTimerEvent__from_json(json##data)) {
            | Ok(res) => handler(res)
            | Error(Some(msg)) => failwith("Error parsing playTimer event: " ++ msg)
            | Error(None) => failwith("Error parsing playTimer event")
        };
    });

/* TODO: Below events use Js.Json.t as payload but I'm not sure what the actual format is */
let onError = (handler: Js.Json.t => unit) => _on(_player, "error", handler);
let onPlaySessionExpired = (handler: Js.Json.t => unit) => _on(_player, "playsessionexpired", handler);
let onMetadata = (handler: Js.Json.t => unit) => _on(_player, "metadata", handler);

[@bs.send.pipe: player] external play : string => unit = "";
