NapsterPlayer.init("api_key", "v2.2");

NapsterPlayer.setAuth({
    "accessToken": "one",
    "refreshToken": "two"
});

NapsterPlayer.signedIn();

NapsterPlayer.on(Ready, () => ());

NapsterPlayer.on(Error, (e) =>
    Js.log(Js.Json.decodeObject(e))
);
