
      var gateway = `ws://${window.location.hostname}/ws`;
      var websocket;
      var rd_slider;
      var set_freq_timeout;
      window.addEventListener('load', onLoad);
      function initWebSocket() {
        console.log('Trying to open a WebSocket connection...');
        websocket = new WebSocket(gateway);
        websocket.onopen    = onOpen;
        websocket.onclose   = onClose;
        websocket.onmessage = onMessage; // <-- add this line
      }
      function onOpen(event) {
        console.log('Connection opened');
		websocket.send(["get_freq", 0]);
      }
      function onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
      }
      function onMessage(event) {
        var state;
        if (event.data == "1"){
          state = "ON";
        }
        else{
          state = "OFF";
        }
        //document.getElementById('state').innerHTML = state;
		console.log(event.data);
      }
      function onLoad(event) {
        initWebSocket();
        init_system();
      }
      function init_system() {
        rd_slider = document.getElementById("radio_slider");
        document.getElementById("next_freq").addEventListener("click",next_freq);
        document.getElementById("prev_freq").addEventListener("click",prev_freq);
        rd_slider.onmouseup = ()=>{set_frequency(rd_slider.value)};
      }
      function next_freq(){
        var f = parseFloat(rd_slider.value) + 0.050000;
        set_frequency(f);
      }
      function prev_freq(){
        var f = parseFloat(rd_slider.value) - 0.050000;
        set_frequency(f);
      }
      function set_frequency(freq){
        rd_slider.value = parseFloat(freq);
        console.log(rd_slider.value);
        document.getElementById("rangeValue").innerHTML =rd_slider.value;
        if(set_freq_timeout>0){
            return;
        }
        set_freq_timeout = 1;
        setTimeout(()=>{
            websocket.send(["freq", rd_slider.value]);
            set_freq_timeout = 0; 
        }, 10);
        
      }