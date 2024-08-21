/*JavaScript*/

import OneTimePassword from "./one-time-password.js";
import OnDemandPassword from "./on-demand-password.js";
import OOBA from "./ooba.js";


export class LogonBox extends Element {
    /**
     * Called when element is attached to the DOM tree
     * 
     *         <div style="text-align: center;">
            <label for="password">Password:</label>

            <input type="password" id="password" name="password" required
                   minlength="4" maxlength="8" size="30"/>
        </div>
     */
    componentDidMount() {
        this.render();
    }

    render() {
        const component = <div>
        <div style="text-align: center;">
            <label >Name:
                <input type="text" size="30" novalue="Enter your login name" value={this.attributes["loginname"]} disabled/>
            </label>
            


        <div style="text-align: center; margin: 0.4rem;">
            <select name="Authenticators" id="authenticator-select">
                <option value="">--Please choose an option--</option>
                <option value="otp">One Time Password</option>
                <option value="odp">On Demand Password</option>
                <option value="ooba">Out of Band Push</option>
            </select>
        </div>

        <div id="authenticator" style="text-align: center; margin: 0.4rem;"></div>

        
        <div style="text-align: center; margin: 0.4rem;">
            <button id="submit" style="width:200px">
                <svg aria-hidden="true" viewBox="0 0 10 10">
                <path d="M7 9L5 8 3 9V6L1 4h3l1-3 1 3h3L7 6z" />
                </svg>
                Submit
            </button>
        </div>            
            
        <div style="text-align: center; margin: 0.4rem;">
            <button id="quit" style="width:200px">Close</button>
        </div>               
            
            
        </div>
      </div>;

        // replace element content with component
        this.content(component);        
    }

    /**
     * On text enter
     * @param {object} _event
     * @param {Element} select
     */
    ["on change at select"](_event, select) {
        console.log(_event.target.value);
        console.log(select.value);
//        const velement = <div id="root"><h1>Hello, world</h1></div>;
        if(select.value == "otp") {
            document.$("div#authenticator").patch(<OneTimePassword />);
        }
        else if (select.value == "odp") {
            document.$("div#authenticator").patch(<OnDemandPassword />);
        }
        else if (select.value == "ooba") {
            document.$("div#authenticator").patch(<OOBA />);
        }        
        //we can just update the corresponding authenticator element
//        let elem = evt.target;
//        let title = elem.state.checked ? "checked" : "unchecked"; 
//        elem.componentUpdate({title: title});
        //this.showSuggestionsFor(input.value);
    }

    ["on click at button#submit"](_event, _element) {
        var url = URL.fromPath("dnalogon.png");
        //should we loadImage first?
        document.$("img#logo").attributes["src"] = url;
        //https://github.com/c-smile/sciter-js-sdk/blob/main/docs/md/Window.md
        Window.this.modal(<info>test</info>);

//        var [sx,sy,sw,sh] = Window.this.screenBox("workarea","rect"); // gettting screen/monitor size
    }

    ["on click at button#quit"](_event, _element) {
        Window.this.close();
    }    

}

/*
function component is not good for event handler, at least I haven't found a sample on that.
export default function LogonBox(props) {
      return (<div>
      </div>);
}
*/

