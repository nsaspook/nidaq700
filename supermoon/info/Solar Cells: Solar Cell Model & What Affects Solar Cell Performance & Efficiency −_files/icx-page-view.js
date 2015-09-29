function icx_writePageView() {

  // 0006824: Do not trigger page view count when article tools displayed on iCopyright servers
    if (typeof icx_perform_pageview == 'undefined') {
        icx_perform_pageview = true;
    }

    if (typeof icx_content_id != 'undefined' && typeof icx_publication_id != 'undefined' && icx_perform_pageview) {
        var uniqueBeaconId = "icx-beacon-"+icx_publication_id+"-"+icx_content_id;
        // Make sure this beacon hasn't been added already.
        if (document.getElementById(uniqueBeaconId) == null) {
            // insert into document
            var port = document.location.protocol == 'https:' ? '443' : '80';
            var text = '<img id="'+uniqueBeaconId+'" class="icx-beacon" style="display:none;" width="1" height="1" border="0" src="' + document.location.protocol + '//license.icopyright.net:' + port + '/publisher/pageView.act?publication_id=' + icx_publication_id + '&content_id=' + icx_content_id + '&random=' + Math.random() + '"/>';
            var div = document.createElement("div");
            div.innerHTML = text;
            document.getElementsByTagName('body')[0].appendChild(div);
        }
    }
}
icx_writePageView();
