
/*
 * Auto-generated file. Do not modify by hand.
 * Run: python3 tools/web_compress.py
 *
 * This HTML code will be injected in /setup webpage using a <div></div> element as parent
 * The parent element will have the HTML id properties equal to 'raw-html-<id>'
 * where the id value will be equal to the id parameter passed to the function addHTML(html_code, id).
 */

static const char custom_html[] PROGMEM = R"EOF(
<div id="iconcontent">
        <div id="form-con">
            <form>
                <label for="lametric-iconID">Icon ID</label><br>
                <input type="text" id="lametric-iconID" name="lametric-iconID"><br>
                <div class="button-row">
                    <input class="btn" type="button" value="Preview" onclick="createLametricLink()">
                    <input class="btn" type="button" value="Download" onclick="downloadLametricImage()">
                </div>
            </form>
        </div>
        <br>
        <br>
        <div id="icon-container">
        </div>
    </div>
)EOF";

static const char custom_css[] PROGMEM = R"EOF(
.iconcontent {
            width: 50%;
            justify-content: center;
        }
        #form-con {
            width: 50%;
            margin: 0 auto;
            min-width: 200px;
        }
        #form-con label {
            color: #8890b0;
            font-size: 13px;
        }
        #form-con input[type=text] {
            background: #111638;
            border: 1px solid #1e2550;
            color: #e8e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            font-size: 14px;
            width: 100%;
        }
        #form-con input[type=text]:focus {
            border-color: #f0b800;
            box-shadow: 0 0 0 2px rgba(240,184,0,.25);
            outline: 0;
        }
        .button-row input {
            width: 50%;
            margin: 0 5px;
        }
        .button-row {
            display: flex;
            justify-content: space-evenly;
            margin: 0 -5px;
            margin-top: 5px;
        }
        #icon-container {
            margin: 0 auto;
            max-width: 150px;
            max-height: 150px;
            width: 150px;
            background-color: black;
            height: 150px;
            margin: 0 auto;
            border-radius: 8px;
            border: 1px solid #1e2550;
        }
        #icon-container img {
            image-rendering: pixelated;
            max-width: 150px;
            max-height: 150px;
            width: 150px;
            background-color: black;
            height: 150px;
            border-radius: 8px;
        }
)EOF";

static const char custom_script[] PROGMEM = R"EOF(
function createLametricLink(){const e=document.getElementById("lametric-iconID").value,t=document.createElement("img");t.onerror=function(){openModalMessage("Error","<b>This ID doesnt exist</b>")},t.src="https://developer.lametric.com/content/apps/icon_thumbs/"+e;const n=document.getElementById("icon-container");n.innerHTML="",n.appendChild(t)}async function downloadLametricImage(){const e=document.getElementById("lametric-iconID").value;try{let n=await fetch("https://developer.lametric.com/content/apps/icon_thumbs/"+e),o=await n.blob();var t="";const c=n.headers.get("content-type");if("image/jpeg"===c||"image/png"===c){t=".jpg";let n=new Image,c=URL.createObjectURL(o);n.onload=function(){let o=document.createElement("canvas");o.width=n.width,o.height=n.height,o.getContext("2d").drawImage(n,0,0,n.width,n.height),o.toBlob((function(n){sendBlob(n,e,t)}),"image/jpeg",1),URL.revokeObjectURL(c)},n.src=c}else"image/gif"===n.headers.get("content-type")&&sendBlob(o,e,t=".gif")}catch(e){console.log("Error"),openModalMessage("Error","<b>This ID doesnt exist</b>")}}function sendBlob(e,t,n){const o=new FormData;o.append("image",e,"ICONS/"+t+n),fetch("/edit",{method:"POST",body:o,mode:"no-cors"}).then((e=>{e.ok&&openModalMessage("Finish","<b>Icon saved</b>")})).catch((e=>{console.log(e)}))}
)EOF";

// screen_html, backup_html, screenfull_html, datafetcher_html removed — served from SPA in LittleFS /web/

static const char update_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'/>
<title>SVITRIX - Update</title>
<style>body{font-family:system-ui,-apple-system,sans-serif;background:#070b1e;color:#e8e8f0;display:flex;justify-content:center;align-items:center;min-height:100vh;margin:0}.c{background:#0a0e27;border:1px solid #1e2550;border-radius:12px;padding:40px;text-align:center;max-width:480px;width:90%}h2{color:#f0b800;margin-bottom:24px}form{margin:16px 0}input[type=file]{color:#e8e8f0;margin:8px 0}input[type=submit]{background:#f0b800;color:#070b1e;border:none;border-radius:8px;padding:12px 24px;font-size:16px;font-weight:600;cursor:pointer;margin-top:8px}input[type=submit]:hover{background:#e8a800}hr{border-color:#1e2550;margin:24px 0}</style>
</head><body><div class='c'><h2>Firmware Update</h2>
<form method='POST' action='/update' enctype='multipart/form-data'>
<input type='file' accept='.bin,.bin.gz' name='update'><br>
<input type='submit' value='Upload Firmware'>
</form></div></body></html>

)EOF";
