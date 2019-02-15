type initOptions = {.
    "catalog": Js.undefined(string),
    "consumerKey": string,
    "player": Js.undefined(string),
    "version": string
};
let init: (~catalog: string=?, ~player: string=?, string, string) => option(Js.Exn.t);
type player;

type setOptions = {.
    "accessToken": string,
    "refreshToken": string
};
let setAuth: setOptions => unit;

let auth: unit => unit;
let tokensSet: unit => bool;
let load: unit => unit;
let testConnection: unit => Js.Promise.t(unit);

let onReady: (player => unit) => unit;
let onPlayStopped: (unit => unit) => unit;

[@decco]
type playEventCode =
    | PlayStarted | BufferFull | PlayComplete | Connected | Paused | Unpaused;
[@decco]
type playEvent = {
    code: playEventCode,
    id: string,
    paused: bool,
    playing: bool,
};
let onPlayEvent: (playEvent => unit) => unit;

[@decco]
type playTimerEvent = {
    currentTime: float,
    totalTime: float,
};
let onPlayTimer: (playTimerEvent => unit) => unit;
let onError: (Js.Json.t => unit) => unit;
let onPlaySessionExpired: (Js.Json.t => unit) => unit;
let onMetadata: (Js.Json.t => unit) => unit;
let play: (string, player) => unit;
let setVolume: (float, player) => unit;
let seek: (float, player) => unit;
let pause: player => unit;
