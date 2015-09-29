/* Fitvids */
(function ($) {

    $.fn.fitVids = function (options) {
        var settings = {
            customSelector:null
        }

        var div = document.createElement('div'),
            ref = document.getElementsByTagName('base')[0] || document.getElementsByTagName('script')[0];

        div.className = 'fit-vids-style';
        div.innerHTML = '&shy;<style>         \
      .fluid-width-video-wrapper {        \
         width: 100%;                     \
         position: relative;              \
         padding: 0;                      \
      }                                   \
                                          \
      .fluid-width-video-wrapper iframe,  \
      .fluid-width-video-wrapper object,  \
      .fluid-width-video-wrapper embed {  \
         position: absolute;              \
         top: 0;                          \
         left: 0;                         \
         width: 100%;                     \
         height: 100%;                    \
      }                                   \
    </style>';

        ref.parentNode.insertBefore(div, ref);

        if (options) {
            $.extend(settings, options);
        }

        return this.each(function () {
            var selectors = [
                "iframe[src*='player.vimeo.com']",
                "iframe[src*='www.youtube.com']",
                "iframe[src*='www.kickstarter.com']",
                "object",
                "embed"
            ];

            if (settings.customSelector) {
                selectors.push(settings.customSelector);
            }

            var $allVideos = $(this).find(selectors.join(','));

            $allVideos.each(function () {
                var $this = $(this);
                if (this.tagName.toLowerCase() == 'embed' && $this.parent('object').length || $this.parent('.fluid-width-video-wrapper').length) {
                    return;
                }
                var height = this.tagName.toLowerCase() == 'object' ? $this.attr('height') : $this.height(),
                    aspectRatio = height / $this.width();
                if (!$this.attr('id')) {
                    var videoID = 'fitvid' + Math.floor(Math.random() * 999999);
                    $this.attr('id', videoID);
                }
                $this.wrap('<div class="fluid-width-video-wrapper"></div>').parent('.fluid-width-video-wrapper').css('padding-top', (aspectRatio * 100) + "%");
                $this.removeAttr('height').removeAttr('width');
            });
        });

    }
})(jQuery);

/* Flexslider v1.8 */
(function (a) {
    a.flexslider = function (c, b) {
        var d = c;
        d.init = function () {
            d.vars = a.extend({}, a.flexslider.defaults, b);
            d.data("flexslider", true);
            d.container = a(".slides", d).first();
            d.slides = a(".slides:first > li", d);
            d.count = d.slides.length;
            d.animating = false;
            d.currentSlide = d.vars.slideToStart;
            d.animatingTo = d.currentSlide;
            d.atEnd = (d.currentSlide == 0) ? true : false;
            d.eventType = ("ontouchstart" in document.documentElement) ? "touchstart" : "click";
            d.cloneCount = 0;
            d.cloneOffset = 0;
            d.manualPause = false;
            d.vertical = (d.vars.slideDirection == "vertical");
            d.prop = (d.vertical) ? "top" : "marginLeft";
            d.args = {};
            d.transitions = "webkitTransition" in document.body.style;
            if (d.transitions) {
                d.prop = "-webkit-transform"
            }
            if (d.vars.controlsContainer != "") {
                d.controlsContainer = a(d.vars.controlsContainer).eq(a(".slides").index(d.container));
                d.containerExists = d.controlsContainer.length > 0
            }
            if (d.vars.manualControls != "") {
                d.manualControls = a(d.vars.manualControls, ((d.containerExists) ? d.controlsContainer : d));
                d.manualExists = d.manualControls.length > 0
            }
            if (d.vars.randomize) {
                d.slides.sort(function () {
                    return(Math.round(Math.random()) - 0.5)
                });
                d.container.empty().append(d.slides)
            }
            if (d.vars.animation.toLowerCase() == "slide") {
                if (d.transitions) {
                    d.setTransition(0)
                }
                d.css({overflow:"hidden"});
                if (d.vars.animationLoop) {
                    d.cloneCount = 2;
                    d.cloneOffset = 1;
                    d.container.append(d.slides.filter(":first").clone().addClass("clone")).prepend(d.slides.filter(":last").clone().addClass("clone"))
                }
                d.newSlides = a(".slides:first > li", d);
                var m = (-1 * (d.currentSlide + d.cloneOffset));
                if (d.vertical) {
                    d.newSlides.css({display:"block", width:"100%", "float":"left"});
                    d.container.height((d.count + d.cloneCount) * 200 + "%").css("position", "absolute").width("100%");
                    setTimeout(function () {
                        d.css({position:"relative"}).height(d.slides.filter(":first").height());
                        d.args[d.prop] = (d.transitions) ? "translate3d(0," + m * d.height() + "px,0)" : m * d.height() + "px";
                        d.container.css(d.args)
                    }, 100)
                } else {
                    d.args[d.prop] = (d.transitions) ? "translate3d(" + m * d.width() + "px,0,0)" : m * d.width() + "px";
                    d.container.width((d.count + d.cloneCount) * 200 + "%").css(d.args);
                    setTimeout(function () {
                        d.newSlides.width(d.width()).css({"float":"left", display:"block"})
                    }, 100)
                }
            } else {
                d.transitions = false;
                d.slides.css({width:"100%", "float":"left", marginRight:"-100%"}).eq(d.currentSlide).fadeIn(d.vars.animationDuration)
            }
            if (d.vars.controlNav) {
                if (d.manualExists) {
                    d.controlNav = d.manualControls
                } else {
                    var e = a('<ol class="flex-control-nav"></ol>');
                    var s = 1;
                    for (var t = 0; t < d.count; t++) {
                        e.append("<li><a>" + s + "</a></li>");
                        s++
                    }
                    if (d.containerExists) {
                        a(d.controlsContainer).append(e);
                        d.controlNav = a(".flex-control-nav li a", d.controlsContainer)
                    } else {
                        d.append(e);
                        d.controlNav = a(".flex-control-nav li a", d)
                    }
                }
                d.controlNav.eq(d.currentSlide).addClass("active");
                d.controlNav.bind(d.eventType, function (i) {
                    i.preventDefault();
                    if (!a(this).hasClass("active")) {
                        (d.controlNav.index(a(this)) > d.currentSlide) ? d.direction = "next" : d.direction = "prev";
                        d.flexAnimate(d.controlNav.index(a(this)), d.vars.pauseOnAction)
                    }
                })
            }
            if (d.vars.directionNav) {
                var v = a('<ul class="flex-direction-nav"><li><a class="prev" href="#">' + d.vars.prevText + '</a></li><li><a class="next" href="#">' + d.vars.nextText + "</a></li></ul>");
                if (d.containerExists) {
                    a(d.controlsContainer).append(v);
                    d.directionNav = a(".flex-direction-nav li a", d.controlsContainer)
                } else {
                    d.append(v);
                    d.directionNav = a(".flex-direction-nav li a", d)
                }
                if (!d.vars.animationLoop) {
                    if (d.currentSlide == 0) {
                        d.directionNav.filter(".prev").addClass("disabled")
                    } else {
                        if (d.currentSlide == d.count - 1) {
                            d.directionNav.filter(".next").addClass("disabled")
                        }
                    }
                }
                d.directionNav.bind(d.eventType, function (i) {
                    i.preventDefault();
                    var j = (a(this).hasClass("next")) ? d.getTarget("next") : d.getTarget("prev");
                    if (d.canAdvance(j)) {
                        d.flexAnimate(j, d.vars.pauseOnAction)
                    }
                })
            }
            if (d.vars.keyboardNav && a("ul.slides").length == 1) {
                function h(i) {
                    if (d.animating) {
                        return
                    } else {
                        if (i.keyCode != 39 && i.keyCode != 37) {
                            return
                        } else {
                            if (i.keyCode == 39) {
                                var j = d.getTarget("next")
                            } else {
                                if (i.keyCode == 37) {
                                    var j = d.getTarget("prev")
                                }
                            }
                            if (d.canAdvance(j)) {
                                d.flexAnimate(j, d.vars.pauseOnAction)
                            }
                        }
                    }
                }

                a(document).bind("keyup", h)
            }
            if (d.vars.mousewheel) {
                d.mousewheelEvent = (/Firefox/i.test(navigator.userAgent)) ? "DOMMouseScroll" : "mousewheel";
                d.bind(d.mousewheelEvent, function (y) {
                    y.preventDefault();
                    y = y ? y : window.event;
                    var i = y.detail ? y.detail * -1 : y.wheelDelta / 40, j = (i < 0) ? d.getTarget("next") : d.getTarget("prev");
                    if (d.canAdvance(j)) {
                        d.flexAnimate(j, d.vars.pauseOnAction)
                    }
                })
            }
            if (d.vars.slideshow) {
                if (d.vars.pauseOnHover && d.vars.slideshow) {
                    d.hover(function () {
                        d.pause()
                    }, function () {
                        if (!d.manualPause) {
                            d.resume()
                        }
                    })
                }
                d.animatedSlides = setInterval(d.animateSlides, d.vars.slideshowSpeed)
            }
            if (d.vars.pausePlay) {
                var q = a('<div class="flex-pauseplay"><span></span></div>');
                if (d.containerExists) {
                    d.controlsContainer.append(q);
                    d.pausePlay = a(".flex-pauseplay span", d.controlsContainer)
                } else {
                    d.append(q);
                    d.pausePlay = a(".flex-pauseplay span", d)
                }
                var n = (d.vars.slideshow) ? "pause" : "play";
                d.pausePlay.addClass(n).text((n == "pause") ? d.vars.pauseText : d.vars.playText);
                d.pausePlay.bind(d.eventType, function (i) {
                    i.preventDefault();
                    if (a(this).hasClass("pause")) {
                        d.pause();
                        d.manualPause = true
                    } else {
                        d.resume();
                        d.manualPause = false
                    }
                })
            }
            if ("ontouchstart" in document.documentElement) {
                var w, u, l, r, o, x, p = false;
                d.each(function () {
                    if ("ontouchstart" in document.documentElement) {
                        this.addEventListener("touchstart", g, false)
                    }
                });
                function g(i) {
                    if (d.animating) {
                        i.preventDefault()
                    } else {
                        if (i.touches.length == 1) {
                            d.pause();
                            r = (d.vertical) ? d.height() : d.width();
                            x = Number(new Date());
                            l = (d.vertical) ? (d.currentSlide + d.cloneOffset) * d.height() : (d.currentSlide + d.cloneOffset) * d.width();
                            w = (d.vertical) ? i.touches[0].pageY : i.touches[0].pageX;
                            u = (d.vertical) ? i.touches[0].pageX : i.touches[0].pageY;
                            d.setTransition(0);
                            this.addEventListener("touchmove", k, false);
                            this.addEventListener("touchend", f, false)
                        }
                    }
                }

                function k(i) {
                    o = (d.vertical) ? w - i.touches[0].pageY : w - i.touches[0].pageX;
                    p = (d.vertical) ? (Math.abs(o) < Math.abs(i.touches[0].pageX - u)) : (Math.abs(o) < Math.abs(i.touches[0].pageY - u));
                    if (!p) {
                        i.preventDefault();
                        if (d.vars.animation == "slide" && d.transitions) {
                            if (!d.vars.animationLoop) {
                                o = o / ((d.currentSlide == 0 && o < 0 || d.currentSlide == d.count - 1 && o > 0) ? (Math.abs(o) / r + 2) : 1)
                            }
                            d.args[d.prop] = (d.vertical) ? "translate3d(0," + (-l - o) + "px,0)" : "translate3d(" + (-l - o) + "px,0,0)";
                            d.container.css(d.args)
                        }
                    }
                }

                function f(j) {
                    d.animating = false;
                    if (d.animatingTo == d.currentSlide && !p && !(o == null)) {
                        var i = (o > 0) ? d.getTarget("next") : d.getTarget("prev");
                        if (d.canAdvance(i) && Number(new Date()) - x < 550 && Math.abs(o) > 20 || Math.abs(o) > r / 2) {
                            d.flexAnimate(i, d.vars.pauseOnAction)
                        } else {
                            d.flexAnimate(d.currentSlide, d.vars.pauseOnAction)
                        }
                    }
                    this.removeEventListener("touchmove", k, false);
                    this.removeEventListener("touchend", f, false);
                    w = null;
                    u = null;
                    o = null;
                    l = null
                }
            }
            if (d.vars.animation.toLowerCase() == "slide") {
                a(window).resize(function () {
                    if (!d.animating) {
                        if (d.vertical) {
                            d.height(d.slides.filter(":first").height());
                            d.args[d.prop] = (-1 * (d.currentSlide + d.cloneOffset)) * d.slides.filter(":first").height() + "px";
                            if (d.transitions) {
                                d.setTransition(0);
                                d.args[d.prop] = (d.vertical) ? "translate3d(0," + d.args[d.prop] + ",0)" : "translate3d(" + d.args[d.prop] + ",0,0)"
                            }
                            d.container.css(d.args)
                        } else {
                            d.newSlides.width(d.width());
                            d.args[d.prop] = (-1 * (d.currentSlide + d.cloneOffset)) * d.width() + "px";
                            if (d.transitions) {
                                d.setTransition(0);
                                d.args[d.prop] = (d.vertical) ? "translate3d(0," + d.args[d.prop] + ",0)" : "translate3d(" + d.args[d.prop] + ",0,0)"
                            }
                            d.container.css(d.args)
                        }
                    }
                })
            }
            d.vars.start(d)
        };
        d.flexAnimate = function (g, f) {
            if (!d.animating) {
                d.animating = true;
                d.animatingTo = g;
                d.vars.before(d);
                if (f) {
                    d.pause()
                }
                if (d.vars.controlNav) {
                    d.controlNav.removeClass("active").eq(g).addClass("active")
                }
                d.atEnd = (g == 0 || g == d.count - 1) ? true : false;
                if (!d.vars.animationLoop && d.vars.directionNav) {
                    if (g == 0) {
                        d.directionNav.removeClass("disabled").filter(".prev").addClass("disabled")
                    } else {
                        if (g == d.count - 1) {
                            d.directionNav.removeClass("disabled").filter(".next").addClass("disabled")
                        } else {
                            d.directionNav.removeClass("disabled")
                        }
                    }
                }
                if (!d.vars.animationLoop && g == d.count - 1) {
                    d.pause();
                    d.vars.end(d)
                }
                if (d.vars.animation.toLowerCase() == "slide") {
                    var e = (d.vertical) ? d.slides.filter(":first").height() : d.slides.filter(":first").width();
                    if (d.currentSlide == 0 && g == d.count - 1 && d.vars.animationLoop && d.direction != "next") {
                        d.slideString = "0px"
                    } else {
                        if (d.currentSlide == d.count - 1 && g == 0 && d.vars.animationLoop && d.direction != "prev") {
                            d.slideString = (-1 * (d.count + 1)) * e + "px"
                        } else {
                            d.slideString = (-1 * (g + d.cloneOffset)) * e + "px"
                        }
                    }
                    d.args[d.prop] = d.slideString;
                    if (d.transitions) {
                        d.setTransition(d.vars.animationDuration);
                        d.args[d.prop] = (d.vertical) ? "translate3d(0," + d.slideString + ",0)" : "translate3d(" + d.slideString + ",0,0)";
                        d.container.css(d.args).one("webkitTransitionEnd transitionend", function () {
                            d.wrapup(e)
                        })
                    } else {
                        d.container.animate(d.args, d.vars.animationDuration, function () {
                            d.wrapup(e)
                        })
                    }
                } else {
                    d.slides.eq(d.currentSlide).fadeOut(d.vars.animationDuration);
                    d.slides.eq(g).fadeIn(d.vars.animationDuration, function () {
                        d.wrapup()
                    })
                }
            }
        };
        d.wrapup = function (e) {
            if (d.vars.animation == "slide") {
                if (d.currentSlide == 0 && d.animatingTo == d.count - 1 && d.vars.animationLoop) {
                    d.args[d.prop] = (-1 * d.count) * e + "px";
                    if (d.transitions) {
                        d.setTransition(0);
                        d.args[d.prop] = (d.vertical) ? "translate3d(0," + d.args[d.prop] + ",0)" : "translate3d(" + d.args[d.prop] + ",0,0)"
                    }
                    d.container.css(d.args)
                } else {
                    if (d.currentSlide == d.count - 1 && d.animatingTo == 0 && d.vars.animationLoop) {
                        d.args[d.prop] = -1 * e + "px";
                        if (d.transitions) {
                            d.setTransition(0);
                            d.args[d.prop] = (d.vertical) ? "translate3d(0," + d.args[d.prop] + ",0)" : "translate3d(" + d.args[d.prop] + ",0,0)"
                        }
                        d.container.css(d.args)
                    }
                }
            }
            d.animating = false;
            d.currentSlide = d.animatingTo;
            d.vars.after(d)
        };
        d.animateSlides = function () {
            if (!d.animating) {
                d.flexAnimate(d.getTarget("next"))
            }
        };
        d.pause = function () {
            clearInterval(d.animatedSlides);
            if (d.vars.pausePlay) {
                d.pausePlay.removeClass("pause").addClass("play").text(d.vars.playText)
            }
        };
        d.resume = function () {
            d.animatedSlides = setInterval(d.animateSlides, d.vars.slideshowSpeed);
            if (d.vars.pausePlay) {
                d.pausePlay.removeClass("play").addClass("pause").text(d.vars.pauseText)
            }
        };
        d.canAdvance = function (e) {
            if (!d.vars.animationLoop && d.atEnd) {
                if (d.currentSlide == 0 && e == d.count - 1 && d.direction != "next") {
                    return false
                } else {
                    if (d.currentSlide == d.count - 1 && e == 0 && d.direction == "next") {
                        return false
                    } else {
                        return true
                    }
                }
            } else {
                return true
            }
        };
        d.getTarget = function (e) {
            d.direction = e;
            if (e == "next") {
                return(d.currentSlide == d.count - 1) ? 0 : d.currentSlide + 1
            } else {
                return(d.currentSlide == 0) ? d.count - 1 : d.currentSlide - 1
            }
        };
        d.setTransition = function (e) {
            d.container.css({"-webkit-transition-duration":(e / 1000) + "s"})
        };
        d.init()
    };
    a.flexslider.defaults = {animation:"fade", slideDirection:"horizontal", slideshow:true, slideshowSpeed:7000, animationDuration:600, directionNav:true, controlNav:true, keyboardNav:true, mousewheel:false, prevText:"&#9664;", nextText:"&#9654;", pausePlay:false, pauseText:"Pause", playText:"Play", randomize:false, slideToStart:0, animationLoop:true, pauseOnAction:true, pauseOnHover:false, controlsContainer:"", manualControls:"", start:function () {
    }, before:function () {
    }, after:function () {
    }, end:function () {
    }};
    a.fn.flexslider = function (b) {
        return this.each(function () {
            if (a(this).find(".slides li").length == 1) {
                a(this).find(".slides li").fadeIn(400)
            } else {
                if (a(this).data("flexslider") != true) {
                    new a.flexslider(a(this), b)
                }
            }
        })
    }
})(jQuery);

// usage: log('inside coolFunc', this, arguments);
window.log = function () {
    log.history = log.history || [];   // store logs to an array for reference
    log.history.push(arguments);
    if (this.console) {
        arguments.callee = arguments.callee.caller;
        console.log(Array.prototype.slice.call(arguments));
    }
};
(function (b) {
    function c() {
    }

    for (var d = "assert,count,debug,dir,dirxml,error,exception,group,groupCollapsed,groupEnd,info,log,markTimeline,profile,profileEnd,time,timeEnd,trace,warn".split(","), a; a = d.pop();)b[a] = b[a] || c
})(window.console = window.console || {});

// jQuery Easing
jQuery.easing["jswing"] = jQuery.easing["swing"];
jQuery.extend(jQuery.easing, {def:"easeOutQuad", swing:function (a, b, c, d, e) {
    return jQuery.easing[jQuery.easing.def](a, b, c, d, e)
}, easeInQuad:function (a, b, c, d, e) {
    return d * (b /= e) * b + c
}, easeOutQuad:function (a, b, c, d, e) {
    return-d * (b /= e) * (b - 2) + c
}, easeInOutQuad:function (a, b, c, d, e) {
    if ((b /= e / 2) < 1)return d / 2 * b * b + c;
    return-d / 2 * (--b * (b - 2) - 1) + c
}, easeInCubic:function (a, b, c, d, e) {
    return d * (b /= e) * b * b + c
}, easeOutCubic:function (a, b, c, d, e) {
    return d * ((b = b / e - 1) * b * b + 1) + c
}, easeInOutCubic:function (a, b, c, d, e) {
    if ((b /= e / 2) < 1)return d / 2 * b * b * b + c;
    return d / 2 * ((b -= 2) * b * b + 2) + c
}, easeInQuart:function (a, b, c, d, e) {
    return d * (b /= e) * b * b * b + c
}, easeOutQuart:function (a, b, c, d, e) {
    return-d * ((b = b / e - 1) * b * b * b - 1) + c
}, easeInOutQuart:function (a, b, c, d, e) {
    if ((b /= e / 2) < 1)return d / 2 * b * b * b * b + c;
    return-d / 2 * ((b -= 2) * b * b * b - 2) + c
}, easeInQuint:function (a, b, c, d, e) {
    return d * (b /= e) * b * b * b * b + c
}, easeOutQuint:function (a, b, c, d, e) {
    return d * ((b = b / e - 1) * b * b * b * b + 1) + c
}, easeInOutQuint:function (a, b, c, d, e) {
    if ((b /= e / 2) < 1)return d / 2 * b * b * b * b * b + c;
    return d / 2 * ((b -= 2) * b * b * b * b + 2) + c
}, easeInSine:function (a, b, c, d, e) {
    return-d * Math.cos(b / e * (Math.PI / 2)) + d + c
}, easeOutSine:function (a, b, c, d, e) {
    return d * Math.sin(b / e * (Math.PI / 2)) + c
}, easeInOutSine:function (a, b, c, d, e) {
    return-d / 2 * (Math.cos(Math.PI * b / e) - 1) + c
}, easeInExpo:function (a, b, c, d, e) {
    return b == 0 ? c : d * Math.pow(2, 10 * (b / e - 1)) + c
}, easeOutExpo:function (a, b, c, d, e) {
    return b == e ? c + d : d * (-Math.pow(2, -10 * b / e) + 1) + c
}, easeInOutExpo:function (a, b, c, d, e) {
    if (b == 0)return c;
    if (b == e)return c + d;
    if ((b /= e / 2) < 1)return d / 2 * Math.pow(2, 10 * (b - 1)) + c;
    return d / 2 * (-Math.pow(2, -10 * --b) + 2) + c
}, easeInCirc:function (a, b, c, d, e) {
    return-d * (Math.sqrt(1 - (b /= e) * b) - 1) + c
}, easeOutCirc:function (a, b, c, d, e) {
    return d * Math.sqrt(1 - (b = b / e - 1) * b) + c
}, easeInOutCirc:function (a, b, c, d, e) {
    if ((b /= e / 2) < 1)return-d / 2 * (Math.sqrt(1 - b * b) - 1) + c;
    return d / 2 * (Math.sqrt(1 - (b -= 2) * b) + 1) + c
}, easeInElastic:function (a, b, c, d, e) {
    var f = 1.70158;
    var g = 0;
    var h = d;
    if (b == 0)return c;
    if ((b /= e) == 1)return c + d;
    if (!g)g = e * .3;
    if (h < Math.abs(d)) {
        h = d;
        var f = g / 4
    } else var f = g / (2 * Math.PI) * Math.asin(d / h);
    return-(h * Math.pow(2, 10 * (b -= 1)) * Math.sin((b * e - f) * 2 * Math.PI / g)) + c
}, easeOutElastic:function (a, b, c, d, e) {
    var f = 1.70158;
    var g = 0;
    var h = d;
    if (b == 0)return c;
    if ((b /= e) == 1)return c + d;
    if (!g)g = e * .3;
    if (h < Math.abs(d)) {
        h = d;
        var f = g / 4
    } else var f = g / (2 * Math.PI) * Math.asin(d / h);
    return h * Math.pow(2, -10 * b) * Math.sin((b * e - f) * 2 * Math.PI / g) + d + c
}, easeInOutElastic:function (a, b, c, d, e) {
    var f = 1.70158;
    var g = 0;
    var h = d;
    if (b == 0)return c;
    if ((b /= e / 2) == 2)return c + d;
    if (!g)g = e * .3 * 1.5;
    if (h < Math.abs(d)) {
        h = d;
        var f = g / 4
    } else var f = g / (2 * Math.PI) * Math.asin(d / h);
    if (b < 1)return-.5 * h * Math.pow(2, 10 * (b -= 1)) * Math.sin((b * e - f) * 2 * Math.PI / g) + c;
    return h * Math.pow(2, -10 * (b -= 1)) * Math.sin((b * e - f) * 2 * Math.PI / g) * .5 + d + c
}, easeInBack:function (a, b, c, d, e, f) {
    if (f == undefined)f = 1.70158;
    return d * (b /= e) * b * ((f + 1) * b - f) + c
}, easeOutBack:function (a, b, c, d, e, f) {
    if (f == undefined)f = 1.70158;
    return d * ((b = b / e - 1) * b * ((f + 1) * b + f) + 1) + c
}, easeInOutBack:function (a, b, c, d, e, f) {
    if (f == undefined)f = 1.70158;
    if ((b /= e / 2) < 1)return d / 2 * b * b * (((f *= 1.525) + 1) * b - f) + c;
    return d / 2 * ((b -= 2) * b * (((f *= 1.525) + 1) * b + f) + 2) + c
}, easeInBounce:function (a, b, c, d, e) {
    return d - jQuery.easing.easeOutBounce(a, e - b, 0, d, e) + c
}, easeOutBounce:function (a, b, c, d, e) {
    if ((b /= e) < 1 / 2.75) {
        return d * 7.5625 * b * b + c
    } else if (b < 2 / 2.75) {
        return d * (7.5625 * (b -= 1.5 / 2.75) * b + .75) + c
    } else if (b < 2.5 / 2.75) {
        return d * (7.5625 * (b -= 2.25 / 2.75) * b + .9375) + c
    } else {
        return d * (7.5625 * (b -= 2.625 / 2.75) * b + .984375) + c
    }
}, easeInOutBounce:function (a, b, c, d, e) {
    if (b < e / 2)return jQuery.easing.easeInBounce(a, b * 2, 0, d, e) * .5 + c;
    return jQuery.easing.easeOutBounce(a, b * 2 - e, 0, d, e) * .5 + d * .5 + c
}})

/* Tabber Tabs */
function tabberObj(argsObj) {
    var arg;
    this.div = null;
    this.classMain = "tabber";
    this.classMainLive = "tabberlive";
    this.classTab = "tabbertab";
    this.classTabDefault = "tabbertabdefault";
    this.classNav = "tabbernav";
    this.classTabHide = "tabbertabhide";
    this.classNavActive = "tabberactive";
    this.titleElements = ['h2', 'h3', 'h4', 'h5', 'h6'];
    this.titleElementsStripHTML = true;
    this.removeTitle = true;
    this.addLinkId = false;
    this.linkIdFormat = '<tabberid>nav<tabnumberone>';
    for (arg in argsObj) {
        this[arg] = argsObj[arg]
    }
    this.REclassMain = new RegExp('\\b' + this.classMain + '\\b', 'gi');
    this.REclassMainLive = new RegExp('\\b' + this.classMainLive + '\\b', 'gi');
    this.REclassTab = new RegExp('\\b' + this.classTab + '\\b', 'gi');
    this.REclassTabDefault = new RegExp('\\b' + this.classTabDefault + '\\b', 'gi');
    this.REclassTabHide = new RegExp('\\b' + this.classTabHide + '\\b', 'gi');
    this.tabs = new Array();
    if (this.div) {
        this.init(this.div);
        this.div = null
    }
}
tabberObj.prototype.init = function (e) {
    var childNodes, i, i2, t, defaultTab = 0, DOM_ul, DOM_li, DOM_a, aId, headingElement;
    if (!document.getElementsByTagName) {
        return false
    }
    if (e.id) {
        this.id = e.id
    }
    this.tabs.length = 0;
    childNodes = e.childNodes;
    for (i = 0; i < childNodes.length; i++) {
        if (childNodes[i].className && childNodes[i].className.match(this.REclassTab)) {
            t = new Object();
            t.div = childNodes[i];
            this.tabs[this.tabs.length] = t;
            if (childNodes[i].className.match(this.REclassTabDefault)) {
                defaultTab = this.tabs.length - 1
            }
        }
    }
    DOM_ul = document.createElement("ul");
    DOM_ul.className = this.classNav;
    for (i = 0; i < this.tabs.length; i++) {
        t = this.tabs[i];
        t.headingText = t.div.title;
        if (this.removeTitle) {
            t.div.title = ''
        }
        if (!t.headingText) {
            for (i2 = 0; i2 < this.titleElements.length; i2++) {
                headingElement = t.div.getElementsByTagName(this.titleElements[i2])[0];
                if (headingElement) {
                    t.headingText = headingElement.innerHTML;
                    if (this.titleElementsStripHTML) {
                        t.headingText.replace(/<br>/gi, " ");
                        t.headingText = t.headingText.replace(/<[^>]+>/g, "")
                    }
                    break
                }
            }
        }
        if (!t.headingText) {
            t.headingText = i + 1
        }
        DOM_li = document.createElement("li");
        t.li = DOM_li;
        DOM_a = document.createElement("a");
        DOM_a.appendChild(document.createTextNode(t.headingText));
        DOM_a.href = "javascript:void(null);";
        DOM_a.title = t.headingText;
        DOM_a.onclick = this.navClick;
        DOM_a.tabber = this;
        DOM_a.tabberIndex = i;
        if (this.addLinkId && this.linkIdFormat) {
            aId = this.linkIdFormat;
            aId = aId.replace(/<tabberid>/gi, this.id);
            aId = aId.replace(/<tabnumberzero>/gi, i);
            aId = aId.replace(/<tabnumberone>/gi, i + 1);
            aId = aId.replace(/<tabtitle>/gi, t.headingText.replace(/[^a-zA-Z0-9\-]/gi, ''));
            DOM_a.id = aId
        }
        DOM_li.appendChild(DOM_a);
        DOM_ul.appendChild(DOM_li)
    }
    e.insertBefore(DOM_ul, e.firstChild);
    e.className = e.className.replace(this.REclassMain, this.classMainLive);
    this.tabShow(defaultTab);
    if (typeof this.onLoad == 'function') {
        this.onLoad({tabber:this})
    }
    return this
};
tabberObj.prototype.navClick = function (event) {
    var rVal, a, self, tabberIndex, onClickArgs;
    a = this;
    if (!a.tabber) {
        return false
    }
    self = a.tabber;
    tabberIndex = a.tabberIndex;
    a.blur();
    if (typeof self.onClick == 'function') {
        onClickArgs = {'tabber':self, 'index':tabberIndex, 'event':event};
        if (!event) {
            onClickArgs.event = window.event
        }
        rVal = self.onClick(onClickArgs);
        if (rVal === false) {
            return false
        }
    }
    self.tabShow(tabberIndex);
    return false
};
tabberObj.prototype.tabHideAll = function () {
    var i;
    for (i = 0; i < this.tabs.length; i++) {
        this.tabHide(i)
    }
};
tabberObj.prototype.tabHide = function (tabberIndex) {
    var div;
    if (!this.tabs[tabberIndex]) {
        return false
    }
    div = this.tabs[tabberIndex].div;
    if (!div.className.match(this.REclassTabHide)) {
        div.className += ' ' + this.classTabHide
    }
    this.navClearActive(tabberIndex);
    return this
};
tabberObj.prototype.tabShow = function (tabberIndex) {
    var div;
    if (!this.tabs[tabberIndex]) {
        return false
    }
    this.tabHideAll();
    div = this.tabs[tabberIndex].div;
    div.className = div.className.replace(this.REclassTabHide, '');
    this.navSetActive(tabberIndex);
    if (typeof this.onTabDisplay == 'function') {
        this.onTabDisplay({'tabber':this, 'index':tabberIndex})
    }
    return this
};
tabberObj.prototype.navSetActive = function (tabberIndex) {
    this.tabs[tabberIndex].li.className = this.classNavActive;
    return this
};
tabberObj.prototype.navClearActive = function (tabberIndex) {
    this.tabs[tabberIndex].li.className = '';
    return this
};
function tabberAutomatic(tabberArgs) {
    var tempObj, divs, i;
    if (!tabberArgs) {
        tabberArgs = {}
    }
    tempObj = new tabberObj(tabberArgs);
    divs = document.getElementsByTagName("div");
    for (i = 0; i < divs.length; i++) {
        if (divs[i].className && divs[i].className.match(tempObj.REclassMain)) {
            tabberArgs.div = divs[i];
            divs[i].tabber = new tabberObj(tabberArgs)
        }
    }
    return this
}
function tabberAutomaticOnLoad(tabberArgs) {
    var oldOnLoad;
    if (!tabberArgs) {
        tabberArgs = {}
    }
    oldOnLoad = document.ready;
    if (typeof document.ready != 'function') {
        document.ready = function () {
            tabberAutomatic(tabberArgs)
        }
    } else {
        document.ready = function () {
            oldOnLoad();
            tabberAutomatic(tabberArgs)
        }
    }
}
if (typeof tabberOptions == 'undefined') {
    tabberAutomaticOnLoad()
} else {
    if (!tabberOptions['manualStartup']) {
        tabberAutomaticOnLoad(tabberOptions)
    }
}

/* Backstretch */
(function (a) {
    a.backstretch = function (b, c, d) {
        function r() {
            if (b) {
                var c;
                var e;
                if (f.length == 0) {
                    f = a("<div />").attr("id", "backstretch").css({left:0, top:0, position:j ? "fixed" : "absolute", overflow:"hidden", zIndex:-999999, margin:0, padding:0, height:"100%", width:"100%"})
                } else {
                    f.find("img").addClass("deleteable")
                }
                c = a("<img />").css({position:"absolute", display:"none", margin:0, padding:0, border:"none", zIndex:-999999}).bind("load",
                    function (b) {
                        var c = a(this), e, h;
                        c.css({width:"auto", height:"auto"});
                        e = this.width || a(b.target).width();
                        h = this.height || a(b.target).height();
                        l = e / h;
                        s();
                        c.fadeIn(g.speed, function () {
                            f.find(".deleteable").remove();
                            if (typeof d == "function")d()
                        })
                    }).appendTo(f);
                e = a("<span />").attr("id", "pattern-filter").appendTo(f);
                if (a("body #backstretch").length == 0) {
                    if (a(window).scrollTop() === 0)window.scrollTo(0, 0);
                    a("body").append(f)
                }
                f.data("settings", g);
                c.attr("src", b);
                a(window).unbind("resize.backstretch").bind("resize.backstretch", function () {
                    if ("onorientationchange"in window) {
                        if (window.pageYOffset === 0)window.scrollTo(0, 1)
                    }
                    s()
                })
            }
        }

        function s() {
            try {
                q = {left:0, top:0}, rootWidth = n = i.width(), rootHeight = k ? window.innerHeight : i.height(), o = n / l;
                if (o >= rootHeight) {
                    p = (o - rootHeight) / 2;
                    if (g.centeredY)q.top = "-" + p + "px"
                } else {
                    o = rootHeight;
                    n = o * l;
                    p = (n - rootWidth) / 2;
                    if (g.centeredX)q.left = "-" + p + "px"
                }
                f.css({width:rootWidth, height:rootHeight}).find("img:not(.deleteable)").css({width:n, height:o}).css(q)
            } catch (a) {
            }
        }

        var e = {centeredX:true, centeredY:true, speed:1e3}, f = a("#backstretch"), g = f.data("settings") || e, h = f.data("settings"), i, j, k, l, m, n, o, p, q;
        if (c && typeof c == "object")a.extend(g, c);
        if (c && typeof c == "function")d = c;
        a(document).ready(function () {
            var b = window, c = navigator.userAgent, d = navigator.platform, e = c.match(/AppleWebKit\/([0-9]+)/), f = !!e && e[1], g = c.match(/Fennec\/([0-9]+)/), h = !!g && g[1], l = c.match(/Opera Mobi\/([0-9]+)/), m = !!l && l[1], n = c.match(/MSIE ([0-9]+)/), o = !!n && n[1];
            j = !((d.indexOf("iPhone") > -1 || d.indexOf("iPad") > -1 || d.indexOf("iPod") > -1) && f && f < 534 || b.operamini && {}.toString.call(b.operamini) === "[object OperaMini]" || l && m < 7458 || c.indexOf("Android") > -1 && f && f < 533 || h && h < 6 || "palmGetResource"in window && f && f < 534 || c.indexOf("MeeGo") > -1 && c.indexOf("NokiaBrowser/8.5.0") > -1 || o && o <= 6);
            i = j ? a(window) : a(document);
            k = j && window.innerHeight;
            r()
        });
        return this
    }
})(jQuery)

/* My Scripts */
jQuery.noConflict();

/* Onload Scripts */
jQuery(window).load(function () {


    // Automate the widths of the tabber tabs widget headers	
    var tabberHeaders = jQuery("ul.tabbernav li");
    var numberOfTabs = tabberHeaders.length;
    var tabberPercentage = 99 / numberOfTabs;
    var tabberPopular = jQuery("div.popular-posts img.wpp-thumbnail").parent("a").parent("li").last();
    var tabberLatest = jQuery("div.widget_recent_entries img.wpp-thumbnail").parent("a").parent("li").last();
    var tabberLastest = jQuery(tabberLatest).children("a").children("img");
    var tabberLast = jQuery(tabberPopular).children("a").children("img");

    jQuery(tabberLast).css("margin-bottom", "0px");
    jQuery(tabberLastest).css("margin-bottom", "0px");

    tabberHeaders.each(function () {
        jQuery(tabberHeaders).css("width", tabberPercentage + "%");
    });


    jQuery('.slides, .flex-direction-nav, .flex-control-nav').hover(function () {
        jQuery('.flex-direction-nav li .next').css('right', '0px');
        jQuery('.flex-direction-nav li .prev').css('left', '-2px');
    }, function () {
        jQuery('.flex-direction-nav li .next').css('right', '-50px');
        jQuery('.flex-direction-nav li .prev').css('left', '-52px');
    });

    // Tidy up the tab widget loading
    jQuery('.tabbertabs').css('height', 'auto');
    jQuery('.sdfsdf').remove();

});

/* Document Ready Scripts */
jQuery(document).ready(function ($omc) {


    // Boot Fitvids.js
    $omc("div.omc-video-container").fitVids();


    // Sort out the tab widget loading
    $omc('.sdfsdf').delay(500).fadeOut(500).delay(500);


    $
    /* omc('div.preloaders').fadeOut(400);  */


    // Move the date-author-stamp beneath the gallery
    var galleryStamp = $omc('p.omc-format-gallery');
    $omc('p.omc-format-gallery').remove();
    $omc('.gallery-to-slideshow-wrapper').after(galleryStamp);


    // Add the "&raquo;" to the submenu 
    jQuery("nav#omc-main-navigation ul li ul.sub-menu li").each(function () {
        if (jQuery(this).children('ul').length > 0) {
            jQuery(this).find('a:first').append('<span class="omc-rarrr"> &raquo;</span>');
        }
    });


    // Boot PrettyPhoto Lightbox
    jQuery("a[rel^='prettyPhoto']").prettyPhoto({
        horizontal_padding:20,
        theme:'light_square'

    });


    // Boot the flexslider
    jQuery('.flexslider').flexslider({
        animation:"fade",
        controlsContainer:".flex-container",
        slideshow:true
    });


    // Fix the width of the slider pagination links
    var flexCircles = $omc('ol.flex-control-nav > li').size();


    if (flexCircles < 9) {
        $omc('ol.flex-control-nav').css("width", "37px");
    }
    else if (flexCircles > 8) {
        $omc('ol.flex-control-nav').css("width", "52px");
    }

    // get rid of the ghost anchors in module a
    $omc('.omc-module-a a:empty').remove();


    // Boot the elastislide
    jQuery('#carousel').elastislide({ });


    // fix the wp caption
    $omc('div.wp-caption').each(function () {
        $omc(this).removeAttr('style')
    });


    // Shortcode animation - Show/Hide
    $omc("a.show_hide").toggle(
        function () {
            $omc(this).parent("div").children("div").fadeIn(300);
        },
        function () {
            $omc(this).parent("div").children("div").fadeOut(300);
        }
    );


    //Code for the Tabs Shortcode
    $omc(".omc-tab-content").hide();
    $omc("ul.omc-tabs li:first-child").addClass("active").show();
    $omc(".omc-tab-container .omc-tab-content:nth-child(2)").show();
    $omc(".omc-tab-container").fadeIn();

    $omc("ul.omc-tabs li").click(function () {
        $omc(this).parent("ul").children("li").removeClass("active");
        $omc(this).addClass("active");
        $omc(this).parent("ul").next("div").children("div").hide();
        var activeTab = $omc(this).find("a").attr("href");
        $omc(activeTab).fadeIn(500);
        return false;
    });


    // effect the color coded menu items	
    var menuItems = $omc('nav#omc-main-navigation ul li');
    $omc("li:has(small)").addClass("has-small");

    $omc(menuItems).hover(function () {

        $omc(this).find('ul:first').fadeIn(1);

        if ($omc(this).hasClass('has-small')) {

            var menuColor = $omc(this).children('small').text();
            var parentAnchor = $omc(this).children('a');

            if (menuColor !== '') {
                $omc(parentAnchor).css('background-color', '#' + menuColor);
                $omc(parentAnchor).css('color', '#FFF');
                $omc(this).find('ul').css('background-color', '#' + menuColor);
            }
        }

    }, function () {

        $omc(this).find('ul:first').fadeOut(1);

        if ($omc(this).hasClass('has-small')) {

            var menuColor = $omc(this).children('small').text();
            var parentAnchor = $omc(this).children('a');

            if (!$omc(this).parent('ul').hasClass('sub-menu')) {
                if ($omc(this).hasClass('current-menu-item')) {
                    $omc(parentAnchor).css('background-color', '#' + menuColor);
                } else if ($omc(this).hasClass('current-category-ancestor')) {
                    $omc(parentAnchor).css('background-color', '#' + menuColor);
                } else if ($omc(this).hasClass('current-menu-parent')) {
                    $omc(parentAnchor).css('background-color', '#' + menuColor);
                } else if ($omc(this).hasClass('current-post-ancestor')) {
                    $omc(parentAnchor).css('background-color', '#' + menuColor);
                } else {
                    $omc(parentAnchor).css('background-color', '#FFF');
                    $omc(parentAnchor).css('color', '#333');
                }
            }
        }
    });


    // Adds navigation to the select element
    $omc("select#omc-mobile-menu").change(function () {
        window.location = $omc(this).find("option:selected").val();
    });


    // Header Menu Search	
    var searchOverlay = $omc("span#omc-search-overlay"),
        searchForm = $omc("form.omc-search-form"),
        searchInput = $omc("input.omc-header-search-input-box"),
        searchButton = $omc("input.omc-header-search-button");

    $omc(searchOverlay).click(function () {
        searchInput.focus();
        searchOverlay.fadeOut(1);
    });

    $omc(searchInput).blur(function () {
        $omc(searchForm).animate({ opacity:0}, 500);
        $omc(searchInput).animate({ width:'104px'}, 500, "easeOutSine");
        $omc(searchButton).animate({ right:'0px'}, 900, "easeOutSine");
        $omc(searchOverlay).fadeIn(400);
    });

    $omc(searchOverlay).mouseover(function () {
        $omc(searchForm).animate({ opacity:1}, 30);
        $omc(searchInput).val("");
        $omc(searchInput).animate({ width:'154px'}, 10, "easeOutSine");
        $omc(searchButton).animate({ right:'-35px'}, 30, "easeOutSine");
    });

    $omc(searchOverlay).mouseout(function () {
        var focusedInputs = $omc(".omc-search-form input:focus");
        if (focusedInputs != null && focusedInputs.length > 0) {
            inputHasFocus = true;
        } else {
            $omc(searchInput).animate({ width:'104px'}, 500, "easeOutSine");
            $omc(searchButton).animate({ right:'0px'}, 900, "easeOutSine");
            $omc(searchForm).animate({ opacity:0}, 500);
            $omc(searchOverlay).fadeIn(400);
        }

    });


    $omc('h4')
        .filter(function () {
            return $omc.trim($omc(this).text()) === ''
        })
        .remove()


    // Social Hovers 
    $omc("a.omc-social-media-icon.large").hover(
        function () {
            $omc(this).children("span.omc-icon").children("span").animate({ opacity:0 }, {speed:5000}, {queue:false});
        },
        function () {
            $omc(this).children("span.omc-icon").children("span").animate({ opacity:1 }, {speed:5000}, {queue:false});
        }
    );


    // hide #back-top first
    $omc("#back-top").hide();


    // fade in #back-top
    $omc(function () {
        $omc(window).scroll(function () {
            if ($omc(this).scrollTop() > 100) {
                $omc('#back-top').fadeIn();
            } else {
                $omc('#back-top').fadeOut();
            }
        });

        // scroll body to 0px on click
        $omc('#back-top a, a.omc-mobile-back-to-top').click(function () {
            $omc('body,html').animate({
                scrollTop:0
            }, 1000, 'easeOutQuint');
            return false;
        });
    });

    //get rid of the bs wp generated image style - stop doing this wordpress
    $omc('div.wp-caption').removeAttr("style");
    $omc('div.wp-caption img').removeAttr("height");
    $omc('#omc-full-article img').removeAttr("height");
    $omc('div.wp-caption img').removeAttr("width");


    // Get rid of the inline styles in the adpress widget
    $omc('li.widget_adpress_widget  ul').css('width', '100%');
    $omc('div#omc-top-banner  ul').css('width', '100%');


    //Knockout the margins on blog style two and module a
    $omc('article.omc-blog-two:odd').css('margin-right', '0px');
    $omc('.omc-module-a').eq(1).css('margin-right', '0px');


    // Fade in the star ratings 
    $omc('span.omc-module-a-stars-under, span.leading-article.omc-module-a-stars-under, span.omc-blog-two-stars-under').css('opacity', '1');
    $omc('#omc-top-banner').fadeIn(200);


    // Sort out the styling of the gallery/video post formats
    $omc('article#omc-full-article h1, article#omc-full-article p.omc-date-time-gallery').fadeIn();
    var galleryHeader = $omc('#omc-full-article.omc-inner-gallery > h1:first');
    var pageHeader = $omc('body.page #omc-full-article > h1:first');
    var galleryDate = $omc('.omc-date-time-gallery');
    var videoDate = $omc('.omc-date-time-video');

    $omc(galleryHeader).remove();
    $omc(galleryDate).remove();
    $omc(videoDate).remove();

    var galleryTrue = $omc('#omc-full-article').has('.gallery-to-slideshow-wrapper');

    if (galleryTrue = false) {
        $omc(pageHeader).remove();
    }

    $omc('#omc-full-article > h1:first').after(videoDate);
    $omc('.gallery-to-slideshow-wrapper').after(galleryHeader);
    $omc('.gallery-to-slideshow-wrapper').after(pageHeader);
    $omc(galleryHeader).after(galleryDate);

    $omc('img.avatar').removeAttr('height');

});


/* PrettyPhoto v 3.1.4 */
(function ($) {
    $.prettyPhoto = {version:'3.1.4'};
    $.fn.prettyPhoto = function (pp_settings) {
        pp_settings = jQuery.extend({hook:'rel', animation_speed:'fast', ajaxcallback:function () {
        }, slideshow:5000, autoplay_slideshow:false, opacity:0.80, show_title:true, allow_resize:true, allow_expand:true, default_width:500, default_height:344, counter_separator_label:'/', theme:'pp_default', horizontal_padding:20, hideflash:false, wmode:'opaque', autoplay:true, modal:false, deeplinking:true, overlay_gallery:true, overlay_gallery_max:30, keyboard_shortcuts:true, changepicturecallback:function () {
        }, callback:function () {
        }, ie6_fallback:true, markup:'<div class="pp_pic_holder"> \
      <div class="ppt">&nbsp;</div> \
      <div class="pp_top"> \
       <div class="pp_left"></div> \
       <div class="pp_middle"></div> \
       <div class="pp_right"></div> \
      </div> \
      <div class="pp_content_container"> \
       <div class="pp_left"> \
       <div class="pp_right"> \
        <div class="pp_content"> \
         <div class="pp_loaderIcon"></div> \
         <div class="pp_fade"> \
          <a href="#" class="pp_expand" title="Expand the image">Expand</a> \
          <div class="pp_hoverContainer"> \
           <a class="pp_next" href="#">next</a> \
           <a class="pp_previous" href="#">previous</a> \
          </div> \
          <div id="pp_full_res"></div> \
          <div class="pp_details"> \
           <div class="pp_nav"> \
            <a href="#" class="pp_arrow_previous">Previous</a> \
            <p class="currentTextHolder">0/0</p> \
            <a href="#" class="pp_arrow_next">Next</a> \
           </div> \
           <p class="pp_description"></p> \
           <div class="pp_social">{pp_social}</div> \
           <a class="pp_close" href="#">Close</a> \
          </div> \
         </div> \
        </div> \
       </div> \
       </div> \
      </div> \
      <div class="pp_bottom"> \
       <div class="pp_left"></div> \
       <div class="pp_middle"></div> \
       <div class="pp_right"></div> \
      </div> \
     </div> \
     <div class="pp_overlay"></div>', gallery_markup:'<div class="pp_gallery"> \
        <a href="#" class="pp_arrow_previous">Previous</a> \
        <div> \
         <ul> \
          {gallery} \
         </ul> \
        </div> \
        <a href="#" class="pp_arrow_next">Next</a> \
       </div>', image_markup:'<img id="fullResImage" src="{path}" />', flash_markup:'<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="{width}" height="{height}"><param name="wmode" value="{wmode}" /><param name="allowfullscreen" value="true" /><param name="allowscriptaccess" value="always" /><param name="movie" value="{path}" /><embed src="{path}" type="application/x-shockwave-flash" allowfullscreen="true" allowscriptaccess="always" width="{width}" height="{height}" wmode="{wmode}"></embed></object>', quicktime_markup:'<object classid="clsid:02BF25D5-8C17-4B23-BC80-D3488ABDDC6B" codebase="http://www.apple.com/qtactivex/qtplugin.cab" height="{height}" width="{width}"><param name="src" value="{path}"><param name="autoplay" value="{autoplay}"><param name="type" value="video/quicktime"><embed src="{path}" height="{height}" width="{width}" autoplay="{autoplay}" type="video/quicktime" pluginspage="http://www.apple.com/quicktime/download/"></embed></object>', iframe_markup:'<iframe src ="{path}" width="{width}" height="{height}" frameborder="no"></iframe>', inline_markup:'<div class="pp_inline">{content}</div>', custom_markup:'', social_tools:'<div class="twitter"><a href="http://twitter.com/share" class="twitter-share-button" data-count="none">Tweet</a><script type="text/javascript" src="http://platform.twitter.com/widgets.js"></script></div><div class="facebook"><iframe src="//www.facebook.com/plugins/like.php?locale=en_US&href={location_href}&amp;layout=button_count&amp;show_faces=true&amp;width=500&amp;action=like&amp;font&amp;colorscheme=light&amp;height=23" scrolling="no" frameborder="0" style="border:none; overflow:hidden; width:500px; height:23px;" allowTransparency="true"></iframe></div>'}, pp_settings);
        var matchedObjects = this, percentBased = false, pp_dimensions, pp_open, pp_contentHeight, pp_contentWidth, pp_containerHeight, pp_containerWidth, windowHeight = $(window).height(), windowWidth = $(window).width(), pp_slideshow;
        doresize = true, scroll_pos = _get_scroll();
        $(window).unbind('resize.prettyphoto').bind('resize.prettyphoto', function () {
            _center_overlay();
            _resize_overlay();
        });
        if (pp_settings.keyboard_shortcuts) {
            $(document).unbind('keydown.prettyphoto').bind('keydown.prettyphoto', function (e) {
                if (typeof $pp_pic_holder != 'undefined') {
                    if ($pp_pic_holder.is(':visible')) {
                        switch (e.keyCode) {
                            case 37:
                                $.prettyPhoto.changePage('previous');
                                e.preventDefault();
                                break;
                            case 39:
                                $.prettyPhoto.changePage('next');
                                e.preventDefault();
                                break;
                            case 27:
                                if (!settings.modal)
                                    $.prettyPhoto.close();
                                e.preventDefault();
                                break;
                        }
                        ;
                    }
                    ;
                }
                ;
            });
        }
        ;
        $.prettyPhoto.initialize = function () {
            settings = pp_settings;
            if (settings.theme == 'pp_default')settings.horizontal_padding = 16;
            if (settings.ie6_fallback && $.browser.msie && parseInt($.browser.version) == 6)settings.theme = "light_square";
            theRel = $(this).attr(settings.hook);
            galleryRegExp = /\[(?:.*)\]/;
            isSet = (galleryRegExp.exec(theRel)) ? true : false;
            pp_images = (isSet) ? jQuery.map(matchedObjects, function (n, i) {
                if ($(n).attr(settings.hook).indexOf(theRel) != -1)return $(n).attr('href');
            }) : $.makeArray($(this).attr('href'));
            pp_titles = (isSet) ? jQuery.map(matchedObjects, function (n, i) {
                if ($(n).attr(settings.hook).indexOf(theRel) != -1)return($(n).find('img').attr('alt')) ? $(n).find('img').attr('alt') : "";
            }) : $.makeArray($(this).find('img').attr('alt'));
            pp_descriptions = (isSet) ? jQuery.map(matchedObjects, function (n, i) {
                if ($(n).attr(settings.hook).indexOf(theRel) != -1)return($(n).attr('title')) ? $(n).attr('title') : "";
            }) : $.makeArray($(this).attr('title'));
            if (pp_images.length > settings.overlay_gallery_max)settings.overlay_gallery = false;
            set_position = jQuery.inArray($(this).attr('href'), pp_images);
            rel_index = (isSet) ? set_position : $("a[" + settings.hook + "^='" + theRel + "']").index($(this));
            _build_overlay(this);
            if (settings.allow_resize)
                $(window).bind('scroll.prettyphoto', function () {
                    _center_overlay();
                });
            $.prettyPhoto.open();
            return false;
        }
        $.prettyPhoto.open = function (event) {
            if (typeof settings == "undefined") {
                settings = pp_settings;
                if ($.browser.msie && $.browser.version == 6)settings.theme = "light_square";
                pp_images = $.makeArray(arguments[0]);
                pp_titles = (arguments[1]) ? $.makeArray(arguments[1]) : $.makeArray("");
                pp_descriptions = (arguments[2]) ? $.makeArray(arguments[2]) : $.makeArray("");
                isSet = (pp_images.length > 1) ? true : false;
                set_position = (arguments[3]) ? arguments[3] : 0;
                _build_overlay(event.target);
            }
            if ($.browser.msie && $.browser.version == 6)$('select').css('visibility', 'hidden');
            if (settings.hideflash)$('object,embed,iframe[src*=youtube],iframe[src*=vimeo]').css('visibility', 'hidden');
            _checkPosition($(pp_images).size());
            $('.pp_loaderIcon').show();
            if (settings.deeplinking)
                setHashtag();
            if (settings.social_tools) {
                facebook_like_link = settings.social_tools.replace('{location_href}', encodeURIComponent(location.href));
                $pp_pic_holder.find('.pp_social').html(facebook_like_link);
            }
            if ($ppt.is(':hidden'))$ppt.css('opacity', 0).show();
            $pp_overlay.show().fadeTo(settings.animation_speed, settings.opacity);
            $pp_pic_holder.find('.currentTextHolder').text((set_position + 1) + settings.counter_separator_label + $(pp_images).size());
            if (typeof pp_descriptions[set_position] != 'undefined' && pp_descriptions[set_position] != "") {
                $pp_pic_holder.find('.pp_description').show().html(unescape(pp_descriptions[set_position]));
            } else {
                $pp_pic_holder.find('.pp_description').hide();
            }
            movie_width = (parseFloat(getParam('width', pp_images[set_position]))) ? getParam('width', pp_images[set_position]) : settings.default_width.toString();
            movie_height = (parseFloat(getParam('height', pp_images[set_position]))) ? getParam('height', pp_images[set_position]) : settings.default_height.toString();
            percentBased = false;
            if (movie_height.indexOf('%') != -1) {
                movie_height = parseFloat(($(window).height() * parseFloat(movie_height) / 100) - 150);
                percentBased = true;
            }
            if (movie_width.indexOf('%') != -1) {
                movie_width = parseFloat(($(window).width() * parseFloat(movie_width) / 100) - 150);
                percentBased = true;
            }
            $pp_pic_holder.fadeIn(function () {
                (settings.show_title && pp_titles[set_position] != "" && typeof pp_titles[set_position] != "undefined") ? $ppt.html(unescape(pp_titles[set_position])) : $ppt.html('&nbsp;');
                imgPreloader = "";
                skipInjection = false;
                switch (_getFileType(pp_images[set_position])) {
                    case'image':
                        imgPreloader = new Image();
                        nextImage = new Image();
                        if (isSet && set_position < $(pp_images).size() - 1)nextImage.src = pp_images[set_position + 1];
                        prevImage = new Image();
                        if (isSet && pp_images[set_position - 1])prevImage.src = pp_images[set_position - 1];
                        $pp_pic_holder.find('#pp_full_res')[0].innerHTML = settings.image_markup.replace(/{path}/g, pp_images[set_position]);
                        imgPreloader.onload = function () {
                            pp_dimensions = _fitToViewport(imgPreloader.width, imgPreloader.height);
                            _showContent();
                        };
                        imgPreloader.onerror = function () {
                            alert('Image cannot be loaded. Make sure the path is correct and image exist.');
                            $.prettyPhoto.close();
                        };
                        imgPreloader.src = pp_images[set_position];
                        break;
                    case'youtube':
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        movie_id = getParam('v', pp_images[set_position]);
                        if (movie_id == "") {
                            movie_id = pp_images[set_position].split('youtu.be/');
                            movie_id = movie_id[1];
                            if (movie_id.indexOf('?') > 0)
                                movie_id = movie_id.substr(0, movie_id.indexOf('?'));
                            if (movie_id.indexOf('&') > 0)
                                movie_id = movie_id.substr(0, movie_id.indexOf('&'));
                        }
                        movie = 'http://www.youtube.com/embed/' + movie_id;
                        (getParam('rel', pp_images[set_position])) ? movie += "?rel=" + getParam('rel', pp_images[set_position]) : movie += "?rel=1";
                        if (settings.autoplay)movie += "&autoplay=1";
                        toInject = settings.iframe_markup.replace(/{width}/g, pp_dimensions['width']).replace(/{height}/g, pp_dimensions['height']).replace(/{wmode}/g, settings.wmode).replace(/{path}/g, movie);
                        break;
                    case'vimeo':
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        movie_id = pp_images[set_position];
                        var regExp = /http:\/\/(www\.)?vimeo.com\/(\d+)/;
                        var match = movie_id.match(regExp);
                        movie = 'http://player.vimeo.com/video/' + match[2] + '?title=0&amp;byline=0&amp;portrait=0';
                        if (settings.autoplay)movie += "&autoplay=1;";
                        vimeo_width = pp_dimensions['width'] + '/embed/?moog_width=' + pp_dimensions['width'];
                        toInject = settings.iframe_markup.replace(/{width}/g, vimeo_width).replace(/{height}/g, pp_dimensions['height']).replace(/{path}/g, movie);
                        break;
                    case'quicktime':
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        pp_dimensions['height'] += 15;
                        pp_dimensions['contentHeight'] += 15;
                        pp_dimensions['containerHeight'] += 15;
                        toInject = settings.quicktime_markup.replace(/{width}/g, pp_dimensions['width']).replace(/{height}/g, pp_dimensions['height']).replace(/{wmode}/g, settings.wmode).replace(/{path}/g, pp_images[set_position]).replace(/{autoplay}/g, settings.autoplay);
                        break;
                    case'flash':
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        flash_vars = pp_images[set_position];
                        flash_vars = flash_vars.substring(pp_images[set_position].indexOf('flashvars') + 10, pp_images[set_position].length);
                        filename = pp_images[set_position];
                        filename = filename.substring(0, filename.indexOf('?'));
                        toInject = settings.flash_markup.replace(/{width}/g, pp_dimensions['width']).replace(/{height}/g, pp_dimensions['height']).replace(/{wmode}/g, settings.wmode).replace(/{path}/g, filename + '?' + flash_vars);
                        break;
                    case'iframe':
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        frame_url = pp_images[set_position];
                        frame_url = frame_url.substr(0, frame_url.indexOf('iframe') - 1);
                        toInject = settings.iframe_markup.replace(/{width}/g, pp_dimensions['width']).replace(/{height}/g, pp_dimensions['height']).replace(/{path}/g, frame_url);
                        break;
                    case'ajax':
                        doresize = false;
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        doresize = true;
                        skipInjection = true;
                        $.get(pp_images[set_position], function (responseHTML) {
                            toInject = settings.inline_markup.replace(/{content}/g, responseHTML);
                            $pp_pic_holder.find('#pp_full_res')[0].innerHTML = toInject;
                            _showContent();
                        });
                        break;
                    case'custom':
                        pp_dimensions = _fitToViewport(movie_width, movie_height);
                        toInject = settings.custom_markup;
                        break;
                    case'inline':
                        myClone = $(pp_images[set_position]).clone().append('<br clear="all" />').css({'width':settings.default_width}).wrapInner('<div id="pp_full_res"><div class="pp_inline"></div></div>').appendTo($('body')).show();
                        doresize = false;
                        pp_dimensions = _fitToViewport($(myClone).width(), $(myClone).height());
                        doresize = true;
                        $(myClone).remove();
                        toInject = settings.inline_markup.replace(/{content}/g, $(pp_images[set_position]).html());
                        break;
                }
                ;
                if (!imgPreloader && !skipInjection) {
                    $pp_pic_holder.find('#pp_full_res')[0].innerHTML = toInject;
                    _showContent();
                }
                ;
            });
            return false;
        };
        $.prettyPhoto.changePage = function (direction) {
            currentGalleryPage = 0;
            if (direction == 'previous') {
                set_position--;
                if (set_position < 0)set_position = $(pp_images).size() - 1;
            } else if (direction == 'next') {
                set_position++;
                if (set_position > $(pp_images).size() - 1)set_position = 0;
            } else {
                set_position = direction;
            }
            ;
            rel_index = set_position;
            if (!doresize)doresize = true;
            if (settings.allow_expand) {
                $('.pp_contract').removeClass('pp_contract').addClass('pp_expand');
            }
            _hideContent(function () {
                $.prettyPhoto.open();
            });
        };
        $.prettyPhoto.changeGalleryPage = function (direction) {
            if (direction == 'next') {
                currentGalleryPage++;
                if (currentGalleryPage > totalPage)currentGalleryPage = 0;
            } else if (direction == 'previous') {
                currentGalleryPage--;
                if (currentGalleryPage < 0)currentGalleryPage = totalPage;
            } else {
                currentGalleryPage = direction;
            }
            ;
            slide_speed = (direction == 'next' || direction == 'previous') ? settings.animation_speed : 0;
            slide_to = currentGalleryPage * (itemsPerPage * itemWidth);
            $pp_gallery.find('ul').animate({left:-slide_to}, slide_speed);
        };
        $.prettyPhoto.startSlideshow = function () {
            if (typeof pp_slideshow == 'undefined') {
                $pp_pic_holder.find('.pp_play').unbind('click').removeClass('pp_play').addClass('pp_pause').click(function () {
                    $.prettyPhoto.stopSlideshow();
                    return false;
                });
                pp_slideshow = setInterval($.prettyPhoto.startSlideshow, settings.slideshow);
            } else {
                $.prettyPhoto.changePage('next');
            }
            ;
        }
        $.prettyPhoto.stopSlideshow = function () {
            $pp_pic_holder.find('.pp_pause').unbind('click').removeClass('pp_pause').addClass('pp_play').click(function () {
                $.prettyPhoto.startSlideshow();
                return false;
            });
            clearInterval(pp_slideshow);
            pp_slideshow = undefined;
        }
        $.prettyPhoto.close = function () {
            if ($pp_overlay.is(":animated"))return;
            $.prettyPhoto.stopSlideshow();
            $pp_pic_holder.stop().find('object,embed').css('visibility', 'hidden');
            $('div.pp_pic_holder,div.ppt,.pp_fade').fadeOut(settings.animation_speed, function () {
                $(this).remove();
            });
            $pp_overlay.fadeOut(settings.animation_speed, function () {
                if ($.browser.msie && $.browser.version == 6)$('select').css('visibility', 'visible');
                if (settings.hideflash)$('object,embed,iframe[src*=youtube],iframe[src*=vimeo]').css('visibility', 'visible');
                $(this).remove();
                $(window).unbind('scroll.prettyphoto');
                clearHashtag();
                settings.callback();
                doresize = true;
                pp_open = false;
                delete settings;
            });
        };
        function _showContent() {
            $('.pp_loaderIcon').hide();
            projectedTop = scroll_pos['scrollTop'] + ((windowHeight / 2) - (pp_dimensions['containerHeight'] / 2));
            if (projectedTop < 0)projectedTop = 0;
            $ppt.fadeTo(settings.animation_speed, 1);
            $pp_pic_holder.find('.pp_content').animate({height:pp_dimensions['contentHeight'], width:pp_dimensions['contentWidth']}, settings.animation_speed);
            $pp_pic_holder.animate({'top':projectedTop, 'left':((windowWidth / 2) - (pp_dimensions['containerWidth'] / 2) < 0) ? 0 : (windowWidth / 2) - (pp_dimensions['containerWidth'] / 2), width:pp_dimensions['containerWidth']}, settings.animation_speed, function () {
                $pp_pic_holder.find('.pp_hoverContainer,#fullResImage').height(pp_dimensions['height']).width(pp_dimensions['width']);
                $pp_pic_holder.find('.pp_fade').fadeIn(settings.animation_speed);
                if (isSet && _getFileType(pp_images[set_position]) == "image") {
                    $pp_pic_holder.find('.pp_hoverContainer').show();
                } else {
                    $pp_pic_holder.find('.pp_hoverContainer').hide();
                }
                if (settings.allow_expand) {
                    if (pp_dimensions['resized']) {
                        $('a.pp_expand,a.pp_contract').show();
                    } else {
                        $('a.pp_expand').hide();
                    }
                }
                if (settings.autoplay_slideshow && !pp_slideshow && !pp_open)$.prettyPhoto.startSlideshow();
                settings.changepicturecallback();
                pp_open = true;
            });
            _insert_gallery();
            pp_settings.ajaxcallback();
        }

        ;
        function _hideContent(callback) {
            $pp_pic_holder.find('#pp_full_res object,#pp_full_res embed').css('visibility', 'hidden');
            $pp_pic_holder.find('.pp_fade').fadeOut(settings.animation_speed, function () {
                $('.pp_loaderIcon').show();
                callback();
            });
        }

        ;
        function _checkPosition(setCount) {
            (setCount > 1) ? $('.pp_nav').show() : $('.pp_nav').hide();
        }

        ;
        function _fitToViewport(width, height) {
            resized = false;
            _getDimensions(width, height);
            imageWidth = width, imageHeight = height;
            if (((pp_containerWidth > windowWidth) || (pp_containerHeight > windowHeight)) && doresize && settings.allow_resize && !percentBased) {
                resized = true, fitting = false;
                while (!fitting) {
                    if ((pp_containerWidth > windowWidth)) {
                        imageWidth = (windowWidth - 200);
                        imageHeight = (height / width) * imageWidth;
                    } else if ((pp_containerHeight > windowHeight)) {
                        imageHeight = (windowHeight - 200);
                        imageWidth = (width / height) * imageHeight;
                    } else {
                        fitting = true;
                    }
                    ;
                    pp_containerHeight = imageHeight, pp_containerWidth = imageWidth;
                }
                ;
                _getDimensions(imageWidth, imageHeight);
                if ((pp_containerWidth > windowWidth) || (pp_containerHeight > windowHeight)) {
                    _fitToViewport(pp_containerWidth, pp_containerHeight)
                }
                ;
            }
            ;
            return{width:Math.floor(imageWidth), height:Math.floor(imageHeight), containerHeight:Math.floor(pp_containerHeight), containerWidth:Math.floor(pp_containerWidth) + (settings.horizontal_padding * 2), contentHeight:Math.floor(pp_contentHeight), contentWidth:Math.floor(pp_contentWidth), resized:resized};
        }

        ;
        function _getDimensions(width, height) {
            width = parseFloat(width);
            height = parseFloat(height);
            $pp_details = $pp_pic_holder.find('.pp_details');
            $pp_details.width(width);
            detailsHeight = parseFloat($pp_details.css('marginTop')) + parseFloat($pp_details.css('marginBottom'));
            $pp_details = $pp_details.clone().addClass(settings.theme).width(width).appendTo($('body')).css({'position':'absolute', 'top':-10000});
            detailsHeight += $pp_details.height();
            detailsHeight = (detailsHeight <= 34) ? 36 : detailsHeight;
            if ($.browser.msie && $.browser.version == 7)detailsHeight += 8;
            $pp_details.remove();
            $pp_title = $pp_pic_holder.find('.ppt');
            $pp_title.width(width);
            titleHeight = parseFloat($pp_title.css('marginTop')) + parseFloat($pp_title.css('marginBottom'));
            $pp_title = $pp_title.clone().appendTo($('body')).css({'position':'absolute', 'top':-10000});
            titleHeight += $pp_title.height();
            $pp_title.remove();
            pp_contentHeight = height + detailsHeight;
            pp_contentWidth = width;
            pp_containerHeight = pp_contentHeight + titleHeight + $pp_pic_holder.find('.pp_top').height() + $pp_pic_holder.find('.pp_bottom').height();
            pp_containerWidth = width;
        }

        function _getFileType(itemSrc) {
            if (itemSrc.match(/youtube\.com\/watch/i) || itemSrc.match(/youtu\.be/i)) {
                return'youtube';
            } else if (itemSrc.match(/vimeo\.com/i)) {
                return'vimeo';
            } else if (itemSrc.match(/\b.mov\b/i)) {
                return'quicktime';
            } else if (itemSrc.match(/\b.swf\b/i)) {
                return'flash';
            } else if (itemSrc.match(/\biframe=true\b/i)) {
                return'iframe';
            } else if (itemSrc.match(/\bajax=true\b/i)) {
                return'ajax';
            } else if (itemSrc.match(/\bcustom=true\b/i)) {
                return'custom';
            } else if (itemSrc.substr(0, 1) == '#') {
                return'inline';
            } else {
                return'image';
            }
            ;
        }

        ;
        function _center_overlay() {
            if (doresize && typeof $pp_pic_holder != 'undefined') {
                scroll_pos = _get_scroll();
                contentHeight = $pp_pic_holder.height(), contentwidth = $pp_pic_holder.width();
                projectedTop = (windowHeight / 2) + scroll_pos['scrollTop'] - (contentHeight / 2);
                if (projectedTop < 0)projectedTop = 0;
                if (contentHeight > windowHeight)
                    return;
                $pp_pic_holder.css({'top':projectedTop, 'left':(windowWidth / 2) + scroll_pos['scrollLeft'] - (contentwidth / 2)});
            }
            ;
        }

        ;
        function _get_scroll() {
            if (self.pageYOffset) {
                return{scrollTop:self.pageYOffset, scrollLeft:self.pageXOffset};
            } else if (document.documentElement && document.documentElement.scrollTop) {
                return{scrollTop:document.documentElement.scrollTop, scrollLeft:document.documentElement.scrollLeft};
            } else if (document.body) {
                return{scrollTop:document.body.scrollTop, scrollLeft:document.body.scrollLeft};
            }
            ;
        }

        ;
        function _resize_overlay() {
            windowHeight = $(window).height(), windowWidth = $(window).width();
            if (typeof $pp_overlay != "undefined")$pp_overlay.height($(document).height()).width(windowWidth);
        }

        ;
        function _insert_gallery() {
            if (isSet && settings.overlay_gallery && _getFileType(pp_images[set_position]) == "image" && (settings.ie6_fallback && !($.browser.msie && parseInt($.browser.version) == 6))) {
                itemWidth = 52 + 5;
                navWidth = (settings.theme == "facebook" || settings.theme == "pp_default") ? 50 : 30;
                itemsPerPage = Math.floor((pp_dimensions['containerWidth'] - 100 - navWidth) / itemWidth);
                itemsPerPage = (itemsPerPage < pp_images.length) ? itemsPerPage : pp_images.length;
                totalPage = Math.ceil(pp_images.length / itemsPerPage) - 1;
                if (totalPage == 0) {
                    navWidth = 0;
                    $pp_gallery.find('.pp_arrow_next,.pp_arrow_previous').hide();
                } else {
                    $pp_gallery.find('.pp_arrow_next,.pp_arrow_previous').show();
                }
                ;
                galleryWidth = itemsPerPage * itemWidth;
                fullGalleryWidth = pp_images.length * itemWidth;
                $pp_gallery.css('margin-left', -((galleryWidth / 2) + (navWidth / 2))).find('div:first').width(galleryWidth + 5).find('ul').width(fullGalleryWidth).find('li.selected').removeClass('selected');
                goToPage = (Math.floor(set_position / itemsPerPage) < totalPage) ? Math.floor(set_position / itemsPerPage) : totalPage;
                $.prettyPhoto.changeGalleryPage(goToPage);
                $pp_gallery_li.filter(':eq(' + set_position + ')').addClass('selected');
            } else {
                $pp_pic_holder.find('.pp_content').unbind('mouseenter mouseleave');
            }
        }

        function _build_overlay(caller) {
            if (settings.social_tools)
                facebook_like_link = settings.social_tools.replace('{location_href}', encodeURIComponent(location.href));
            settings.markup = settings.markup.replace('{pp_social}', '');
            $('body').append(settings.markup);
            $pp_pic_holder = $('.pp_pic_holder'), $ppt = $('.ppt'), $pp_overlay = $('div.pp_overlay');
            if (isSet && settings.overlay_gallery) {
                currentGalleryPage = 0;
                toInject = "";
                for (var i = 0; i < pp_images.length; i++) {
                    if (!pp_images[i].match(/\b(jpg|jpeg|png|gif)\b/gi)) {
                        classname = 'default';
                        img_src = '';
                    } else {
                        classname = '';
                        img_src = pp_images[i];
                    }
                    toInject += "<li class='" + classname + "'><a href='#'><img src='" + img_src + "' width='50' alt='' /></a></li>";
                }
                ;
                toInject = settings.gallery_markup.replace(/{gallery}/g, toInject);
                $pp_pic_holder.find('#pp_full_res').after(toInject);
                $pp_gallery = $('.pp_pic_holder .pp_gallery'), $pp_gallery_li = $pp_gallery.find('li');
                $pp_gallery.find('.pp_arrow_next').click(function () {
                    $.prettyPhoto.changeGalleryPage('next');
                    $.prettyPhoto.stopSlideshow();
                    return false;
                });
                $pp_gallery.find('.pp_arrow_previous').click(function () {
                    $.prettyPhoto.changeGalleryPage('previous');
                    $.prettyPhoto.stopSlideshow();
                    return false;
                });
                $pp_pic_holder.find('.pp_content').hover(function () {
                    $pp_pic_holder.find('.pp_gallery:not(.disabled)').fadeIn();
                }, function () {
                    $pp_pic_holder.find('.pp_gallery:not(.disabled)').fadeOut();
                });
                itemWidth = 52 + 5;
                $pp_gallery_li.each(function (i) {
                    $(this).find('a').click(function () {
                        $.prettyPhoto.changePage(i);
                        $.prettyPhoto.stopSlideshow();
                        return false;
                    });
                });
            }
            ;
            if (settings.slideshow) {
                $pp_pic_holder.find('.pp_nav').prepend('<a href="#" class="pp_play">Play</a>')
                $pp_pic_holder.find('.pp_nav .pp_play').click(function () {
                    $.prettyPhoto.startSlideshow();
                    return false;
                });
            }
            $pp_pic_holder.attr('class', 'pp_pic_holder ' + settings.theme);
            $pp_overlay.css({'opacity':0, 'height':$(document).height(), 'width':$(window).width()}).bind('click', function () {
                if (!settings.modal)$.prettyPhoto.close();
            });
            $('a.pp_close').bind('click', function () {
                $.prettyPhoto.close();
                return false;
            });
            if (settings.allow_expand) {
                $('a.pp_expand').bind('click', function (e) {
                    if ($(this).hasClass('pp_expand')) {
                        $(this).removeClass('pp_expand').addClass('pp_contract');
                        doresize = false;
                    } else {
                        $(this).removeClass('pp_contract').addClass('pp_expand');
                        doresize = true;
                    }
                    ;
                    _hideContent(function () {
                        $.prettyPhoto.open();
                    });
                    return false;
                });
            }
            $pp_pic_holder.find('.pp_previous, .pp_nav .pp_arrow_previous').bind('click', function () {
                $.prettyPhoto.changePage('previous');
                $.prettyPhoto.stopSlideshow();
                return false;
            });
            $pp_pic_holder.find('.pp_next, .pp_nav .pp_arrow_next').bind('click', function () {
                $.prettyPhoto.changePage('next');
                $.prettyPhoto.stopSlideshow();
                return false;
            });
            _center_overlay();
        }

        ;
        if (!pp_alreadyInitialized && getHashtag()) {
            pp_alreadyInitialized = true;
            hashIndex = getHashtag();
            hashRel = hashIndex;
            hashIndex = hashIndex.substring(hashIndex.indexOf('/') + 1, hashIndex.length - 1);
            hashRel = hashRel.substring(0, hashRel.indexOf('/'));
            setTimeout(function () {
                $("a[" + pp_settings.hook + "^='" + hashRel + "']:eq(" + hashIndex + ")").trigger('click');
            }, 50);
        }
        return this.unbind('click.prettyphoto').bind('click.prettyphoto', $.prettyPhoto.initialize);
    };
    function getHashtag() {
        url = location.href;
        hashtag = (url.indexOf('#prettyPhoto') !== -1) ? decodeURI(url.substring(url.indexOf('#prettyPhoto') + 1, url.length)) : false;
        return hashtag;
    }

    ;
    function setHashtag() {
        if (typeof theRel == 'undefined')return;
        location.hash = theRel + '/' + rel_index + '/';
    }

    ;
    function clearHashtag() {
        if (location.href.indexOf('#prettyPhoto') !== -1)location.hash = "prettyPhoto";
    }

    function getParam(name, url) {
        name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
        var regexS = "[\\?&]" + name + "=([^&#]*)";
        var regex = new RegExp(regexS);
        var results = regex.exec(url);
        return(results == null) ? "" : results[1];
    }
})(jQuery);
var pp_alreadyInitialized = false;

/* Elastislide */
(function (a, b, c) {
    b.fn.touchwipe = function (a) {
        var c = {min_move_x:20, min_move_y:20, wipeLeft:function () {
        }, wipeRight:function () {
        }, wipeUp:function () {
        }, wipeDown:function () {
        }, preventDefaultEvents:true};
        if (a)b.extend(c, a);
        this.each(function () {
            function e() {
                this.removeEventListener("touchmove", f);
                a = null;
                d = false
            }

            function f(f) {
                if (c.preventDefaultEvents) {
                    f.preventDefault()
                }
                if (d) {
                    var g = f.touches[0].pageX;
                    var h = f.touches[0].pageY;
                    var i = a - g;
                    var j = b - h;
                    if (Math.abs(i) >= c.min_move_x) {
                        e();
                        if (i > 0) {
                            c.wipeLeft()
                        } else {
                            c.wipeRight()
                        }
                    } else if (Math.abs(j) >= c.min_move_y) {
                        e();
                        if (j > 0) {
                            c.wipeDown()
                        } else {
                            c.wipeUp()
                        }
                    }
                }
            }

            function g(c) {
                if (c.touches.length == 1) {
                    a = c.touches[0].pageX;
                    b = c.touches[0].pageY;
                    d = true;
                    this.addEventListener("touchmove", f, false)
                }
            }

            var a;
            var b;
            var d = false;
            if ("ontouchstart"in document.documentElement) {
                this.addEventListener("touchstart", g, false)
            }
        });
        return this
    };
    b.elastislide = function (a, c) {
        this.jQueryel = b(c);
        this._init(a)
    };
    b.elastislide.defaults = {speed:450, easing:"easeOutQuad", imageW:186, margin:31, border:0, minItems:3, current:0, onClick:""};
    b.elastislide.prototype = {_init:function (a) {
        this.options = b.extend(true, {}, b.elastislide.defaults, a);
        this.jQueryslider = this.jQueryel.find("ul");
        this.jQueryitems = this.jQueryslider.children("li");
        this.itemsCount = this.jQueryitems.length;
        this.jQueryesCarousel = this.jQueryslider.parent();
        this._validateOptions();
        this._configure();
        this._addControls();
        this._initEvents();
        this.jQueryslider.show();
        this._slideToCurrent(false)
    }, _validateOptions:function () {
        if (this.options.speed < 0)this.options.speed = 450;
        if (this.options.margin < 0)this.options.margin = 4;
        if (this.options.border < 0)this.options.border = 1;
        if (this.options.minItems < 1 || this.options.minItems > this.itemsCount)this.options.minItems = 1;
        if (this.options.current > this.itemsCount - 1)this.options.current = 0
    }, _configure:function () {
        this.current = this.options.current;
        this.visibleWidth = this.jQueryesCarousel.width();
        if (this.visibleWidth < this.options.minItems * (this.options.imageW + 2 * this.options.border) + (this.options.minItems - 1) * this.options.margin) {
            this._setDim((this.visibleWidth - (this.options.minItems - 1) * this.options.margin) / this.options.minItems);
            this._setCurrentValues();
            this.fitCount = this.options.minItems
        } else {
            this._setDim();
            this._setCurrentValues()
        }
        this.jQueryslider.css({width:this.sliderW})
    }, _setDim:function (a) {
        this.jQueryitems.css({marginRight:this.options.margin, width:a ? a : this.options.imageW + 2 * this.options.border}).children("a").css({borderWidth:this.options.border})
    }, _setCurrentValues:function () {
        this.itemW = this.jQueryitems.outerWidth(true);
        this.sliderW = this.itemW * this.itemsCount;
        this.visibleWidth = this.jQueryesCarousel.width();
        this.fitCount = Math.floor(this.visibleWidth / this.itemW)
    }, _addControls:function () {
        this.jQuerynavNext = b('<span class="es-nav-next">Next</span>');
        this.jQuerynavPrev = b('<span class="es-nav-prev">Previous</span>');
        b('<div class="es-nav"/>').append(this.jQuerynavPrev).append(this.jQuerynavNext).appendTo(this.jQueryel)
    }, _toggleControls:function (a, b) {
        if (a && b) {
            if (b === 1)a === "right" ? this.jQuerynavNext.show() : this.jQuerynavPrev.show(); else a === "right" ? this.jQuerynavNext.hide() : this.jQuerynavPrev.hide()
        } else if (this.current === this.itemsCount - 1 || this.fitCount >= this.itemsCount)this.jQuerynavNext.hide()
    }, _initEvents:function () {
        var c = this;
        b(a).bind("resize.elastislide", function (a) {
            c._setCurrentValues();
            if (c.visibleWidth < c.options.minItems * (c.options.imageW + 2 * c.options.border) + (c.options.minItems - 1) * c.options.margin) {
                c._setDim((c.visibleWidth - (c.options.minItems - 1) * c.options.margin) / c.options.minItems);
                c._setCurrentValues();
                c.fitCount = c.options.minItems
            } else {
                c._setDim();
                c._setCurrentValues()
            }
            c.jQueryslider.css({width:c.sliderW + 10});
            clearTimeout(c.resetTimeout);
            c.resetTimeout = setTimeout(function () {
                c._slideToCurrent()
            }, 200)
        });
        this.jQuerynavNext.bind("click.elastislide", function (a) {
            c._slide("right")
        });
        this.jQuerynavPrev.bind("click.elastislide", function (a) {
            c._slide("left")
        });
        this.jQueryitems.bind("click.elastislide", function (a) {
            c.options.onClick(b(this));
            return false
        });
        c.jQueryslider.touchwipe({wipeLeft:function () {
            c._slide("right")
        }, wipeRight:function () {
            c._slide("left")
        }})
    }, _slide:function (a, d, e, f) {
        if (this.jQueryslider.is(":animated"))return false;
        var g = parseFloat(this.jQueryslider.css("margin-left"));
        if (d === c) {
            var h = this.fitCount * this.itemW, d;
            if (h < 0)return false;
            if (a === "right" && this.sliderW - (Math.abs(g) + h) < this.visibleWidth) {
                h = this.sliderW - (Math.abs(g) + this.visibleWidth) - this.options.margin;
                this._toggleControls("right", -1);
                this._toggleControls("left", 1)
            } else if (a === "left" && Math.abs(g) - h < 0) {
                h = Math.abs(g);
                this._toggleControls("left", -1);
                this._toggleControls("right", 1)
            } else {
                var i;
                a === "right" ? i = Math.abs(g) + this.options.margin + Math.abs(h) : i = Math.abs(g) - this.options.margin - Math.abs(h);
                if (i > 0)this._toggleControls("left", 1); else this._toggleControls("left", -1);
                if (i < this.sliderW - this.visibleWidth)this._toggleControls("right", 1); else this._toggleControls("right", -1)
            }
            a === "right" ? d = "-=" + h : d = "+=" + h
        } else {
            var i = Math.abs(d);
            if (Math.max(this.sliderW, this.visibleWidth) - i < this.visibleWidth) {
                d = -(Math.max(this.sliderW, this.visibleWidth) - this.visibleWidth);
                if (d !== 0)d += this.options.margin;
                this._toggleControls("right", -1);
                i = Math.abs(d)
            }
            if (i > 0)this._toggleControls("left", 1); else this._toggleControls("left", -1);
            if (Math.max(this.sliderW, this.visibleWidth) - this.visibleWidth > i + this.options.margin)this._toggleControls("right", 1); else this._toggleControls("right", -1)
        }
        b.fn.applyStyle = e === c ? b.fn.animate : b.fn.css;
        var j = {marginLeft:d};
        var k = this;
        this.jQueryslider.applyStyle(j, b.extend(true, [], {duration:this.options.speed, easing:this.options.easing, complete:function () {
            if (f)f.call()
        }}))
    }, _slideToCurrent:function (a) {
        var b = this.current * this.itemW;
        this._slide("", -b, a)
    }, add:function (a, b) {
        this.jQueryitems = this.jQueryitems.add(a);
        this.itemsCount = this.jQueryitems.length;
        this._setDim();
        this._setCurrentValues();
        this.jQueryslider.css({width:this.sliderW});
        this._slideToCurrent();
        if (b)b.call(a)
    }, destroy:function (a) {
        this._destroy(a)
    }, _destroy:function (c) {
        this.jQueryel.unbind(".elastislide").removeData("elastislide");
        b(a).unbind(".elastislide");
        if (c)c.call()
    }};
    var d = function (a) {
        if (this.console) {
            console.error(a)
        }
    };
    b.fn.elastislide = function (a) {
        if (typeof a === "string") {
            var c = Array.prototype.slice.call(arguments, 1);
            this.each(function () {
                var e = b.data(this, "elastislide");
                if (!e) {
                    d("cannot call methods on elastislide prior to initialization; " + "attempted to call method '" + a + "'");
                    return
                }
                if (!b.isFunction(e[a]) || a.charAt(0) === "_") {
                    d("no such method '" + a + "' for elastislide instance");
                    return
                }
                e[a].apply(e, c)
            })
        } else {
            this.each(function () {
                var c = b.data(this, "elastislide");
                if (!c) {
                    b.data(this, "elastislide", new b.elastislide(a, this))
                }
            })
        }
        return this
    }
})(window, jQuery)

;
(function ($) {
    function user_rating() {
        if ($('.omc-user-review-rating').length) {
          // Get elements
          this.el = this.build_el();
          if (!this.is_rated()) {
              // Interface fixes
              this.el.stars.top.css('background-position-y', '1px');
              this.el.stars.under.css('width', '100px');
              // Bind Events
              this.bind_events();
          } else {
              this.display_user_rating();
          }
        }
    }

    user_rating.prototype.is_rated = function () {
        if (this.readCookie('gonzo_rating_' + gonzo_script.post_id) === 'rated') {
            return true;
        } else {
            return false;
        }
    };

    user_rating.prototype.display_user_rating = function () {
        var score = this.readCookie('gonzo_rating_score_' + gonzo_script.post_id),
            position = this.readCookie('gonzo_rating_position_'+ gonzo_script.post_id);
        this.el.rating.score.html(score);
        this.el.stars.top.css('width', position + '%');
        this.el.rating.label.your_rating.show();
        this.el.rating.label.user_rating.hide();
    };

    user_rating.prototype.build_el = function () {
        var el = {
            rating:{
                score:$('SPAN.score', '.omc-user-review-description'),
                count:$('SPAN.count', '.omc-user-review-description'),
                label:{
                    your_rating:$('SPAN.your_rating', '.omc-user-review-description'),
                    user_rating:$('SPAN.user_rating', '.omc-user-review-description')
                }
            },
            stars:{
                under:$('.omc-criteria-star-under', '.omc-user-review-rating'),
                top:$('.omc-criteria-star-top', '.omc-user-review-rating')
            }
        };

        // Plain JS style retrieval
        el.stars.old_position = parseInt(el.stars.top[0].style.width, 10);
        el.rating.old_score = el.rating.score.html();

        return el;
    };

    user_rating.prototype.bind_events = function () {
        var me = this;

        // Hover effect
        me.el.stars.under.on('mouseover', function () {
            // changes the sprite
            me.el.stars.top.css('background-position-y', '-20px');

            // Changes the cursor
            $(this).css('cursor', 'pointer');

            // changes the text
            me.el.rating.label.your_rating.show();
            me.el.rating.label.user_rating.hide();

        });
        me.el.stars.under.on('mouseout', function () {
            // Returns the sprite
            me.el.stars.top.css('background-position-y', '1px');

            // Returns the initial position
            me.el.stars.top.css('width', me.el.stars.old_position + '%');

            // Returns the text and initial rating
            me.el.rating.label.user_rating.show();
            me.el.rating.label.your_rating.hide();
            me.el.rating.score.html(me.el.rating.old_score);

        });
        me.el.stars.under.on('mousemove', function (e) {
            if (!e.offsetX){
                e.offsetX = e.clientX - $(e.target).offset().left;
            }
            // Moves the width
            var offset = e.offsetX + 4;
            if (offset > 100) {
                offset = 100;
            }
            me.el.stars.top.css('width', offset + '%');

            // Update the real-time score
            var score = Math.floor(((offset / 10) * 5)) / 10;
            if (score > 5) {
                score = 5;
            }
            me.el.rating.score.html(score);

        });

        // Click effect
        me.el.stars.under.on('click', function (e) {
            if (!e.offsetX){
                e.offsetX = e.clientX - $(e.target).offset().left;
            }
            var count = parseInt(me.el.rating.count.html(), 10) + 1,
                score = (Math.floor(((e.offsetX + 4) / 10) * 5) / 10),
                position = e.offsetX + 4;
            if (score > 5) {
                score = 5;
            }
            if (position > 100) {
                position = 100;
            }
            // Unbind events
            me.el.stars.under.off();
            me.el.stars.under.css('cursor', 'default');

            // Stars animation
            me.el.stars.top.fadeOut(function () {
                me.el.stars.top.css('background-position-y', '0');
                me.el.stars.top.fadeIn();
            });

            // Count increment
            me.el.rating.count.html(count);

            // AJAX call to wordpress
            var req = {
                action:'gonzo_rating',
                rating_position:position,
                rating_score:score,
                post_id:gonzo_script.post_id
            };

            $.post(gonzo_script.ajaxurl, req, function () {
                // Save cookie
                me.createCookie('gonzo_rating_' + gonzo_script.post_id, 'rated', 900);
                me.createCookie('gonzo_rating_score_' + gonzo_script.post_id, score, 900);
                me.createCookie('gonzo_rating_position_' + gonzo_script.post_id, position, 900);
            })
        });
    };

    user_rating.prototype.createCookie = function (name, value, days) {
        if (days) {
            var date = new Date();
            date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
            var expires = "; expires=" + date.toGMTString();
        }
        else var expires = "";
        document.cookie = name + "=" + value + expires + "; path=/";
    }

    user_rating.prototype.readCookie = function (name) {
        var nameEQ = name + "=";
        var ca = document.cookie.split(';');
        for (var i = 0; i < ca.length; i++) {
            var c = ca[i];
            while (c.charAt(0) == ' ') c = c.substring(1, c.length);
            if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
        }
        return null;
    }

    user_rating.prototype.eraseCookie = function (name) {
        createCookie(name, "", -1);
    }

    $(document).ready(function () {
        new user_rating();
    });
})(jQuery);