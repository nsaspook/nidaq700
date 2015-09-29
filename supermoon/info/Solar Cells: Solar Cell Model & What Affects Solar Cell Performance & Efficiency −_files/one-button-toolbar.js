if (typeof iCopyright == 'undefined' || !(typeof(icx_writeToolbar) == 'function')) {
    var iCopyright = {
        insertStyles: function(url) {
            if ("https:" == document.location.protocol) {
                url = url.replace('http://', 'https://');
                url = url.replace(':80/', ':443/');
            }

            var link = document.createElement('link');
            link.href = url;
            link.rel = 'stylesheet';
            link.type = 'text/css';
            document.getElementsByTagName('head')[0].appendChild(link);
        },
        insertScript:function (url) {
            if ("https:" == document.location.protocol) {
                url = url.replace('http://', 'https://');
                url = url.replace(':80/', ':443/');
            }
            try {
                // inserting via DOM fails in Safari 2.0, so brute force approach
                document.write('<script type="text/javascript" src="' + url + '"></script>');
            } catch (e) {
                // for xhtml+xml served content, fall back to DOM methods
                var script = document.createElement('script');
                script.src = url;
                script.type = 'text/javascript';
                document.getElementsByTagName('head')[0].appendChild(script);
            }
        }
    };
    var icx_orientation = 'one-button';
    iCopyright.insertStyles('//d2uzdrx7k4koxz.cloudfront.net/rights/style/one-button-toolbar.css?v=20150924-0027');

    if (typeof jQuery == 'undefined') {
        iCopyright.insertScript('${web.jquery}');
    }

    var docLoc = document.location.protocol;
    if (docLoc && docLoc == "https:") {
    	iCopyright.insertScript('//7993b512cdc62a7916b2-107572ee72ababf2b924b481010d10d7.ssl.cf1.rackcdn.com/icx-pub-' + icx_publication_id + '.js');
    } else {
    	iCopyright.insertScript('//icx.icopyright.net/icx-pub-' + icx_publication_id + '.js');
    }
    
    iCopyright.insertScript('//d2uzdrx7k4koxz.cloudfront.net/rights/js/icx-functions.js?v=20150924-0027');
    iCopyright.insertScript('//d2uzdrx7k4koxz.cloudfront.net/rights/js/icx-one-button-toolbar.js?v=20150924-0027');
    iCopyright.insertScript('//d2uzdrx7k4koxz.cloudfront.net/rights/js/icx-page-view.js?v=20150924-0027');
} else {
	icx_writeToolbar('Republish Reprint', false);
    icx_writePageView();
}
