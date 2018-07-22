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
        "catalog": Js.Undefined.fromOption(catalog),
        "player": Js.Undefined.fromOption(player)
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

type _member;
[@bs.module "napster"] external _member : _member = "member";

[@noserialize]
type setOptions = {.
    "accessToken": string,
    "refreshToken": string
};

[@bs.send] external _setAuth : (_member, setOptions) => unit = "set";
[@bs.send] external _auth : player => unit = "auth";
[@bs.send] external _signedIn : _member => bool = "signedIn";
[@bs.send] external _load : _member => unit = "load";

let setAuth = (opts) => _setAuth(_member, opts);
let auth = () => _auth(_player);
let tokensSet = () => _signedIn(_member);
let load = () => _load(_member);

let testConnection = () => Bluebird.make((~resolve, ~reject as _) => {
    Api.get(true, "/me", (res) => {
        switch (Js.Nullable.toOption(res##error)) {
            | Some(error) => failwith(error)
            | None => resolve()
        };
    });
});

let onReady = (handler) => _on(_player, "ready", () => handler(_player));
let onPlayStopped = (handler: unit => unit) => _on(_player, "playstopped", handler);

[@decco]
type playEventCode =
    | PlayStarted
    | BufferFull
    | PlayComplete
    | Connected
    | Paused
    | Unpaused;

[@decco]
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
            | "PlayStarted" => PlayStarted
            | "BufferFull" => BufferFull
            | "Paused" => Paused
            | "Unpaused" => Unpaused
            | _ => failwith("Unrecognized event: " ++ json##data##code)
        },
        id: json##data##id,
        paused: json##data##paused,
        playing: json##data##playing
    }));
};

[@decco]
type playTimerEvent = {
    currentTime: float,
    totalTime: float
};
let onPlayTimer = (handler) =>
    _on(_player, "playtimer", (json) => {
        switch (playTimerEvent__from_json(json##data)) {
            | Ok(v) => handler(v)
            | Error(e) => Js.log(e); Js.Exn.raiseError("Error decoding playTimerEvent")
        };
    });

/* TODO: Below events use Js.Json.t as payload but I'm not sure what the actual format is */
let onError = (handler: Js.Json.t => unit) => _on(_player, "error", handler);
let onPlaySessionExpired = (handler: Js.Json.t => unit) => _on(_player, "playsessionexpired", handler);
let onMetadata = (handler: Js.Json.t => unit) => _on(_player, "metadata", handler);

[@bs.send.pipe: player] external play : string => unit = "";
[@bs.send.pipe: player] external setVolume : float => unit = "";
[@bs.send.pipe: player] external seek : float => unit = "";
[@bs.send] external pause : player => unit = "";
