if (typeof iCopyrightIcn == 'undefined') {
    var iCopyrightIcn = {
        icx_writeCopyrightNotice: function() {
            if (typeof icx_publication_id == 'undefined') {
                return;
            }
            if (typeof icx_tag_type != 'undefined' && icx_tag_type == 'GENERAL_PURPOSE_FETCHER') {
                if (typeof icx_preview != 'undefined' && icx_preview == 'true') {
                    var tag = '1.' + icx_publication_id;
                } else {
                    var tag = '3.' + icx_publication_id + '?icx_gpf_url=' + document.location.href;
                }
            } else if ((typeof icx_content_id == 'undefined') || (icx_content_id == '[ARTICLE_ID]')) {
                var tag = '1.' + icx_publication_id;
            }
            else {
                var tag = '3.' + icx_publication_id + '?icx_id=' + icx_content_id;
            }
            if (typeof icx_copyright_notice == 'undefined') {
                return;
            }
            // Add the word "Copyright" if it's not already so prefixed
            if (!(icx_copyright_notice.indexOf('Copyright ') === 0)) {
                icx_copyright_notice = 'Copyright ' + icx_copyright_notice;
            }
            var str = '';
            str += '<div class="icx-interactive-notice"><div class="icx-calltoaction">';
            str += '<a href="http://license.icopyright.net:80/rights/tag.act?tag=' + tag + '" title="Main menu of all reuse options" target="_blank" onclick="iCopyrightUtility.openLicenseWindow(this.href); return false;">Click here for reuse options!</a>';
            str += '</div>';
            str += '<div class="icx-note">' + icx_copyright_notice + '</div>';
            str += '</div>';
            document.write(str);
        }
    };
}

iCopyrightIcn.icx_writeCopyrightNotice();