type initOptions = {.
    "consumerKey": string,
    "version": string,
    "catalog": Js.undefined(string),
    "player": Js.undefined(string)
};

[@bs.module "@ryb73/napster"] external init : initOptions => unit = "init";

let doInit = (catalog, player, consumerKey, version) => {
    init({
        "consumerKey": consumerKey,
        "version": version,
        "catalog": Js.Undefined.fromOption(catalog),
        "player": Js.Undefined.fromOption(player)
    });

    None
};

let init = (~catalog=?, ~player=?, consumerKey, version) =>
    try (doInit(catalog, player, consumerKey, version)) {
        | Js.Exn.Error(e) => Some(e)
    };

type player;
[@bs.module "@ryb73/napster"] external player : player = "player";
[@bs.send] external on : (player, string, _ => unit) => unit = "on";

type member;
[@bs.module "@ryb73/napster"] external member : member = "member";

[@noserialize]
type setOptions = {.
    "accessToken": string,
    "refreshToken": string
};

[@bs.send] external setAuth : (member, setOptions) => unit = "set";
[@bs.send] external auth : player => unit = "auth";
[@bs.send] external signedIn : member => bool = "signedIn";
[@bs.send] external load : member => unit = "load";

let setAuth = (opts) => setAuth(member, opts);
let auth = () => auth(player);
let tokensSet = () => signedIn(member);
let load = () => load(member);

let testConnection = () => Reduice.Promise.make((~resolve, ~reject as _) => {
    Api.get(true, "/me", (res) => {
        switch (Js.Nullable.toOption(res##error)) {
            | Some(error) => failwith(error)
            | None => let u = (); [@bs] resolve(u)
        };
    });
});

let onReady = (handler) => on(player, "ready", () => handler(player));
let onPlayStopped = (handler: unit => unit) => on(player, "playstopped", handler);

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
    on(player, "playevent", (json) => handler({
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
    on(player, "playtimer", (json) => {
        switch (playTimerEvent_decode(json##data)) {
            | Ok(v) => handler(v)
            | Error(e) => Js.log(e); Js.Exn.raiseError("Error decoding playTimerEvent")
        };
    });

/* TODO: Below events use Js.Json.t as payload but I'm not sure what the actual format is */
let onError = (handler: Js.Json.t => unit) => on(player, "error", handler);
let onPlaySessionExpired = (handler: Js.Json.t => unit) => on(player, "playsessionexpired", handler);
let onMetadata = (handler: Js.Json.t => unit) => on(player, "metadata", handler);

[@bs.send.pipe: player] external play : string => unit = "";
[@bs.send.pipe: player] external setVolume : float => unit = "";
[@bs.send.pipe: player] external seek : float => unit = "";
[@bs.send] external pause : player => unit = "";
