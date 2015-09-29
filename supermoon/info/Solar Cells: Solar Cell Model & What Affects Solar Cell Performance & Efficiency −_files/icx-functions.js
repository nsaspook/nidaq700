if (typeof jQuery == 'undefined') {
    iCopyright.insertScript('//ajax.googleapis.com/ajax/libs/jquery/1.4.2/jquery.min.js');
}

var iCopyrightUtility = {
    selectText: function() {
        var selection;
        if (window.getSelection) {
            selection = window.getSelection();
            selection = this.refineSelection(selection);
        } else if (document.getSelection) {
            selection = document.getSelection();
            selection = this.refineSelection(selection);
        } else if (document.selection) {
            // should come last; Opera! This is also for IE(8 at least).  This requires the .text method.
            selection = document.selection.createRange().htmlText;
        }
        var sel = "";
        try {
            if (selection != null) {
                if (selection.htmlText) {
                    sel = selection.htmlText;
                } else if (selection.text) {
                    sel = selection.text;
                } else {
                    sel = selection;
                }
            }
            return sel;
        } catch(err) {
            return sel;
        }
    },
    // Returns the HTML from the selection
    refineSelection: function(sel) {
        var range;
        if (sel.getRangeAt) {
            if (sel.rangeCount == 0) {
                // No selected range after all; abort
                return "";
            } else {
                range = sel.getRangeAt(0);
            }
        } else {
            range = document.createRange();
            range.setStart(sel.anchorNode, sel.anchorOffset);
            range.setEnd(sel.focusNode, sel.focusOffset);
        }
        // And the HTML:
        var clonedSelection = range.cloneContents();
        var div = document.createElement('div');
        div.appendChild(clonedSelection);
        sel = div.innerHTML;
        return sel;
    },

    // Remove smart quotes and the like
    // Based on http://diveintogreasemonkey.org/download/dumbquotes.user.js
    cleanupSelection: function(sel) {
        var t = new String(sel);
        var replacements = {
            "\n\n": "</p><p>",
            "\r\n\r\n": "</p><p>",
            "\xa0": " ",
            "\xa9": "&copy;",
            "\xae": "&reg;",
            "\xb7": "*",
            "\u2018": "'",
            "\u2019": "'",
            "\u201c": '"',
            "\u201d": '"',
            "\u2026": "...",
            "\u2022": "&bull;",
            "\u2002": " ",
            "\u2003": " ",
            "\u2009": " ",
            "\u2013": "-",
            "\u2014": "&mdash;",
            "\u2122": "&trade;"};
        for (key in replacements) {
            regex = new RegExp(key, 'g');
            t = t.replace(regex, replacements[key]);
        }
        return "<p>" + t + "</p>";
    },
    // Quick and dirty word count estimate
    estimateWordCount: function(sel) {
        var wdz = sel; // init variable
        wdz = wdz.replace(/<(.|\n)+?>/g, ''); // replace html tags with nothinglyness
        wdz = wdz.replace(/[^'!\.\?a-zA-Z0-9 ]/g, ' '); // replace non-word characters with a space
        wdz = wdz.split(/\s+/); // split words
        for (var i = 0, count = 0; i < wdz.length; i++) {
            if (wdz[i].length > 0) {
                count++;
            }
        }
        return count;
    },
    openLicenseWindow: function(url) {
        if (typeof iCopyrightEZExcerpt != 'undefined') {
            iCopyrightEZExcerpt.stopEZExcerpt();
        }
        var popup = window.open(url, 'contentservices', 'width=511, height=550, scrollbars=yes, resizable=yes');
        popup.focus();
    }
}
