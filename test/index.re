open Js.Promise;

Napster.init "api_key" "v2.2";

Napster.setAuth {
    "accessToken": "one",
    "refreshToken": "two"
};

Napster.signedIn ();

Napster.Api.me ()
    |> then_ (fun data => {
        Js.log data;
        resolve ();
    });

Napster.on Ready (fun () => ());

Napster.on Error (fun e => {
    Js.log @@ Js.Json.decodeObject e;
});