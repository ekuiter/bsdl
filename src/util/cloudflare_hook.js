// bypassing Cloudflare protection - see gist.github.com/antoligy/f4f084b87946f84a89b4

var url = require('system').args[1];
var timeout = 10000;
var page = require('webpage').create();
page.settings.userAgent = require('system').args[2];

page.open(url, function(status) {
    if (status !== "success")
        phantom.exit(1);
    
    setTimeout(function() {
        var clearance = phantom.cookies.
            map(function(cookie) {
                return cookie.name + "=" + cookie.value
            }).
            join("; ");
        
        console.log(clearance);
	phantom.exit();
    }, timeout);
});
