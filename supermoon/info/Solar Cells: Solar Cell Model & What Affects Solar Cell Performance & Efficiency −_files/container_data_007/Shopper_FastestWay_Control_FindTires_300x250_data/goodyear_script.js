
function shimmerIn(){
	TweenMax.to(ctaBtnOver, 0, {opacity:0, x:0, delay: 0});	
	TweenMax.to(ctaBtnOver, .3, {opacity:1, x:61, ease:Sine.easeIn, delay: 0});
	TweenMax.to(ctaBtnOver, .3, {opacity:0, x:122, ease:Sine.easeOut, delay: .3});
}

/////////////Animation

function startAd() {
    
    /*========================  CLICKTAG  =============================*/
    
    // var clickTag = "http://www.Goodyear.com/";
    // var adBtn = document.getElementById("ad");

    // function OPENW(){
    //     window.open(clickTag);
    // }

    // adBtn.addEventListener("click", OPENW, false);
    /*================================================================*/
    
    var adContainer = document.getElementById("container");

    TweenMax.defaultOverwrite = "false";
    document.getElementById("banner").style.visibility = "visible";

    animation1();
	addButtonRollover();
}

function animation1(){
    setTimeout (function(){animation2()}, 3);
};

function animation2(){
	TweenMax.to(invisibleOver, 0, {opacity:1, ease:Expo.easeInOut, delay: 0});
    TweenMax.to(background, 0, {opacity:1, ease:Expo.easeInOut, delay: 0});
	TweenMax.to(logoTop, 0, {opacity:1, ease:Expo.easeInOut, delay: 0});
	TweenMax.to(imgTireShadow, .75, {opacity:1, ease:Expo.easeInOut, delay: 0});
	TweenMax.to(imgTire, 1.2, {y:222, ease:Bounce.easeOut, delay: 0});
	TweenMax.to(imgTireReflection, 1.2, {y:-200, ease:Bounce.easeOut, delay: 0});
    TweenMax.to(txt1, 1.4, {opacity:1, x:-155, ease:Expo.easeInOut, delay: .5});
	TweenMax.to(txt2, 1.4, {opacity:1, x:-155, ease:Expo.easeInOut, delay: 1});
	TweenMax.to(txt3, 1.4, {opacity:1, x:-155, ease:Expo.easeInOut, delay: 1.5, onComplete: animation3});
};

function animation3(){
    TweenMax.to(txt1, 1.5, {opacity:0, ease:Expo.easeInOut, delay: 0});
	TweenMax.to(txt2, 1.5, {opacity:0, ease:Expo.easeInOut, delay: 0});
	TweenMax.to(txt3, 1.5, {opacity:0, ease:Expo.easeInOut, delay: 0});
	
	TweenMax.to(txt4, 1.4, {opacity:1, x:-155, ease:Expo.easeInOut, delay: .5});
	TweenMax.to(txt5, 1.4, {opacity:1, x:-155, ease:Expo.easeInOut, delay: 1});
	TweenMax.to(txt6, 1.4, {opacity:1, x:-155, ease:Expo.easeInOut, delay: 1.5, onComplete: shimmerIn});
	
    TweenMax.to(ctaBtn, 1, {opacity:1, delay:3.8});
};


function addButtonRollover(){
   	invisibleOver.addEventListener("mouseover", shimmerIn, false);
};