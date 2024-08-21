export default function OOBA(attributes) {
  return (
  <div id="authenticator" style="text-align: center; margin: 0.4rem;">
    <fieldset>
        <legend>Select a mobile phone:</legend>

        <div>
          <input type="radio" id="huey" name="drone" value="huey"
                checked />
          <label for="huey">iPhone Huey</label>
        </div>

        <div>
          <input type="radio" id="dewey" name="drone" value="dewey" />
          <label for="dewey">Android Dewey</label>
        </div>

        <div>
          <input type="radio" id="louie" name="drone" value="louie" />
          <label for="louie">iPhone Louie</label>
        </div>
    </fieldset>
  </div>);
}
