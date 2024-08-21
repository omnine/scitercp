
export default class OnDemandPassword extends Element 
{
  componentDidMount() {
//    this.content(<h1>OnDemandPassword!</h1>);
  }

  render() {
    return (<div id="authenticator" style="text-align: center; margin: 0.4rem;">
      <img src="sms.png" width="64" height="64" class="sprite"/>
      <img src="email.png" width="64" height="64" class="sprite"/>
      <img src="telephone.png" width="64" height="64" class="sprite"/>
      <img src="twitter.png" width="64" height="64" class="sprite"/>
    </div>);
  }

    /**
     * On img click event
     * @param {object} _event
     * @param {Element} _element
     */
  ["on click at img"](_event, _element) {
    fetch('http://www.user.com/index.php',{
      method:'POST',ache:'no-cache',headers:{
      //'Content-Type': 'application/json'
      'Content-Type':'application/x-www-form-urlencoded',
      },body:formData
    }).then(function(response){
      out(response.status)
      return response.json();
    }).then(function(data){
      out(data)
    }).catch(function(error){
      //some ..
    })
  }

}

/*
const formData = {};
 
formData.username = 'abc123';
formData.avatar = theFileToUpload;

await fetch('https://example.com/profile/avatar', {
  method: 'POST',
  body: formData
});





export default function OneTimePassword(attributes) {
  return (
  <div>
    <label for="otp">One Time Password:</label>

    <input type="text" id="otp" name="otp" required
           minlength="4" maxlength="8" size="10"/>
  </div>);
}
*/