function icx_writeToolbar(icx_show_text, isFooter) {
  if (typeof icx_publication_id == 'undefined') {
    return;
  }
  if (typeof icx_tag_type != 'undefined' && icx_tag_type == 'GENERAL_PURPOSE_FETCHER') {
    if (typeof icx_preview != 'undefined' && icx_preview == 'true') {
      var tag = '1.' + icx_publication_id;
    } else {
      var tag = '3.' + icx_publication_id + '?icx_gpf_url=' + document.location.href;
    }
  } else if (typeof icx_content_id != 'undefined') {
    var tag = '3.' + icx_publication_id + '?icx_id=' + icx_content_id;
  } else if (typeof icx_url != 'undefined') {
    var tag = '3.' + icx_publication_id + '?icx_url=' + icx_url;
  } else {
    var tag = '1.' + icx_publication_id;
  }
  var str = '';
  var toolbarId = "icx-toolbar-" + Math.floor(Math.random() * 1000);
  str += '<div class="icx-toolbar" id="' + toolbarId + '"><div class="icx-toolbar-inner">';

  str += '<div class="icx-logo">';
  str += '<a class="no_inherit" href="http://license.icopyright.net:80/rights/oneButtonTag.act?tag=' + tag +
      '" target="_blank" onclick="iCopyrightUtility.openLicenseWindow(this.getAttribute(\'href\'));return false;"><div class="icx-logo-text" ' + 
      (isFooter ? 'style="width: 150px !important; padding-left: 35px !important; text-align: left;"' : '') + '>' + icx_show_text + '</div></a>';
  str += '</div>';

  str += '</div><div style="display: none;' + (isFooter ? 'bottom: 5px; text-align: left;' : 'top: 30px;') + '" class="icx-tool-wrapper">';
  //
  // Service id mappings
  //
//            // -- Reprints
//      11,   // Free Print
//      14,   // Instant Print
//      31,   // Delivered Prints
//      17,   // Customized Prints
//
//            // - Republish Online
//      35,   // - free webprint service
//      15,   // Post full article
//      41,   // - hosted eprint service by page view
//      42,   // Post PDF of full article
//      18,   // - custom eprint service, currently called "Customized PDF"
//      36,   // Post Excerpt
//
//            // - Republish Offline
//      28,   // Excerpt Article for print
//      105,  // Republish in newspaper ???
//      107,  // Republish in book
//      22    // Other Permissions and Services
//
//            // - Get syndication feed
//      101   // Get syndication feed

  if (typeof icx_srv_grp_id != 'undefined' && icx_srv_grp_id.indexOf(3) >= 0 && typeof icx_srv_id != 'undefined' && (icx_srv_id.indexOf(15) >= 0 || icx_srv_id.indexOf(42) >= 0 || icx_srv_id.indexOf(36) >= 0 || icx_srv_id.indexOf(35) >= 0 || icx_srv_id.indexOf(41) >= 0 || icx_srv_id.indexOf(18) >= 0)) {
    wrd = 'Republish Online';
    str += '<div class="icx-tool" id="icx-republish-online">';
    str += '<a class="no_inherit" href="http://license.icopyright.net:80/rights/republishOnlineServiceGroup.act?tag=' + tag +
        '" target="_blank" onclick="iCopyrightUtility.openLicenseWindow(this.getAttribute(\'href\'));return false;">' + wrd + '</a>';
    str += '</div>';
  }

  if (typeof icx_srv_grp_id != 'undefined' && icx_srv_grp_id.indexOf(4) >= 0 && typeof icx_srv_id != 'undefined' && (icx_srv_id.indexOf(28) >= 0 || icx_srv_id.indexOf(105) >= 0 || icx_srv_id.indexOf(107) >= 0 || icx_srv_id.indexOf(22) >= 0)) {
    wrd = 'Republish Offline'
    str += '<div class="icx-tool" id="icx-republish-offline">';
    str += '<a class="no_inherit" href="http://license.icopyright.net:80/rights/republishOfflineServiceGroup.act?tag=' + tag +
        '" target="_blank" onclick="iCopyrightUtility.openLicenseWindow(this.getAttribute(\'href\'));return false;">' + wrd + '</a>';
    str += '</div>';
  }

  if (typeof icx_srv_grp_id != 'undefined' && icx_srv_grp_id.indexOf(2) >= 0 && typeof icx_srv_id != 'undefined' && (icx_srv_id.indexOf(11) >= 0 || icx_srv_id.indexOf(14) >= 0 || icx_srv_id.indexOf(31) >= 0 || icx_srv_id.indexOf(17) >= 0)) {
    wrd = 'Reprint'
    str += '<div class="icx-tool" id="icx-reprints">';
    str += '<a class="no_inherit" href="http://license.icopyright.net:80/rights/reprintsServiceGroup.act?tag=' + tag +
        '" target="_blank" onclick="iCopyrightUtility.openLicenseWindow(this.getAttribute(\'href\'));return false;">' + wrd + '</a>';
    str += '</div>';
  }

  if (typeof icx_srv_grp_id != 'undefined' && icx_srv_grp_id.indexOf(4) >= 0 && typeof icx_srv_id != 'undefined' && (icx_srv_id.indexOf(101) >= 0)) {
    wrd = 'Get Syndication Feed'
    str += '<div class="icx-tool" id="icx-syndication">';
    str += '<a class="no_inherit" href="http://repubhub.icopyright.net/search.act?search=yes&showImmediate=yes&dateFilter=THIRTY_DAYS&featuredPublicationRestrictionString=p-' + icx_publication_id + '&additionalPublication=' + icx_publication_id +
        '" target="_blank">' + wrd + '</a>';
    str += '</div>';
  }

  str += '</div>';
  str += '</div>';

  document.write(str);

  // customize background.
  if (typeof icx_bg_color != 'undefined' && icx_bg_color == 'TRANSPARENT') {
    jQuery('.icx-tool-end').css('background', 'none');
    jQuery('.icx-toolbar-inner').css('background', 'none');
  }

  var mouseleaveTimout = null;
  var icx_one_button_open = false;
  jQuery("#" + toolbarId + " .icx-logo a").hover(function () {
    if (mouseleaveTimout != null) {
      window.clearTimeout(mouseleaveTimout);
    } else {
      var toolbar = jQuery(this).parents(".icx-toolbar");
      var toolbarActions = jQuery(".icx-tool-wrapper", toolbar);

      // Make sure the toolbar popup is on the screen.  We only have to worry about it
      // extending off the right side of the screen.
      toolbarActions.css('left', '');
      var innerWidth = window.innerWidth || document.documentElement.clientWidth || document.body.clientWidth;
      var toolbarActionsWidth = toolbarActions.outerWidth(true);
      var toolbarActionsAvailableSpace = innerWidth - jQuery(this).offset().left;
      if (toolbarActionsAvailableSpace < toolbarActionsWidth) {
        toolbarActions.css('left', toolbarActionsAvailableSpace - toolbarActionsWidth - 15);
      }
      toolbarActions.slideDown();
      icx_one_button_open = true;
    }
  }, function () {
  });

  jQuery("#" + toolbarId + " .icx-tool-wrapper").mouseleave(function () {
    var icxToolWrapper = jQuery(this);
    mouseleaveTimout = setTimeout(function () {
      icxToolWrapper.hide();
      mouseleaveTimout = null;
    }, 500);
  });
  jQuery("#" + toolbarId + " .icx-logo a").mouseleave(function () {
    var icxToolWrapper = jQuery("#" + toolbarId + " .icx-tool-wrapper");
    mouseleaveTimout = setTimeout(function () {
      icxToolWrapper.hide();
      mouseleaveTimout = null;
    }, 500);
  });

  jQuery("#" + toolbarId + " .icx-tool-wrapper").mouseenter(function () {
    if (mouseleaveTimout != null) {
      window.clearTimeout(mouseleaveTimout);
    }
  });

  // mobile touch support.  hide the popup when a touch event occurs
  document.addEventListener('touchmove', function(event) {
    if (icx_one_button_open == true) {
      var icxToolWrapper = jQuery("#" + toolbarId + " .icx-tool-wrapper");
      icxToolWrapper.hide();
      mouseleaveTimout = null;
    }
  }, false);


}

// Define indexOf if there is none (I'm lookin' at you IE);
// see http://stackoverflow.com/questions/3629183/why-doesnt-indexof-work-on-an-array-ie8
if (!Array.prototype.indexOf) {
  Array.prototype.indexOf = function (elt /*, from*/) {
    var len = this.length >>> 0;
    var from = Number(arguments[1]) || 0;
    from = (from < 0) ? Math.ceil(from) : Math.floor(from);
    if (from < 0) {
      from += len;
    }
    for (; from < len; from++) {
      if (from in this && this[from] === elt) {
        return from;
      }
    }
    return -1;
  };
}

icx_writeToolbar('Republish Reprint', false);
