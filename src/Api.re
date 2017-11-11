open Js.Result;
open Js.Promise;

type api;
external _api : api = "api" [@@bs.module "napster"];
external _get : api => Js.boolean => string => (Js.Json.t => unit) => unit = "get" [@@bs.send];

let _performGet path => {
    Js.Promise.make @@ fun ::resolve reject::_ => {
        _get _api Js.true_ path @@ fun res => {
            switch (Js.Json.decodeObject res) {
                | None => resolve (Ok res) [@bs]

                | Some dict =>
                    switch (Js.Dict.get dict "error") {
                        | None => resolve (Ok res) [@bs]
                        | Some _ => resolve (Error res) [@bs]
                    }
            };
        };
    };
};

type member = {
    id: string,
    realName: string,
    screenName: string,
    bio: option string,
    location: option string,
    gender: option string,
    visibility: string,
    role: string,
    followingCount: int,
    followerCount: int,
    avatar: string,
    avatarId: string,
    defaultAvatar: string, /* "true" or "false" */
    avatarVersion: int
};

type me = {
    me: member
};

let me () => {
    _performGet "/me"
        |> then_ (fun res => {
            switch res {
                | Error e => resolve (Error e)
                | Ok v => switch (me__from_json v) {
                    | Error (Some key) => resolve (Error (Js.Json.string @@ "Deserialization error (key: " ^ key ^ ")"))
                    | Error _ => resolve (Error (Js.Json.string "Deserialization error"))
                    | Ok v => resolve (Ok v)
                }
            };
        });
};