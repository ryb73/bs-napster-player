NapsterPlayer.init("api_key", "v2.2");

NapsterPlayer.setAuth({
    "accessToken": "one",
    "refreshToken": "two"
});

NapsterPlayer.tokensSet() === true;

NapsterPlayer.onReady(NapsterPlayer.play("tra.123"));

NapsterPlayer.onError((e) => Js.log(Js.Json.decodeObject(e)));
