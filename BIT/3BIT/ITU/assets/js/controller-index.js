/**
 * ITU project 
 * Author: Natália Holková (xholko02)
 */

// Login page functions
/**
 * Redirect to page with tasks
 */
function redirect() {
    var login = document.forms["login-form"]["login-username"].value;
    var pass = document.forms["login-form"]["login-pass"].value;

    if (verifyLoginCredentials(login, pass)) {
        window.location.replace("tasks.html");
        return true;
    }
    else {
        window.history.back();
        return false;
    }
}

/**
 * Redirect from register to page with tasks
 */
function redirectRegister() {
    var login = document.forms["register-form"]["register-username"].value;
    var pass = document.forms["register-form"]["register-pass"].value;
    var email = document.forms["register-form"]["register-email"].value;

    if (verifyRegister(login, email) == true) {
        window.location.replace("tasks.html");
        return true;
    }
    else {
        window.location.replace("index.html");
        return false;
    }
}

/**
 * Verify entered login credentials
 * 
 * @param {*} login Entered username
 * @param {*} pass Entered password
 */
function verifyLoginCredentials(login, pass) {
    var user = JSON.parse(localStorage.getItem("storeUser"));

    if (user.login == login && user.password == pass) 
        return true;
    else
        return false;
}

function verifyRegister(login, email) {
    var user = JSON.parse(localStorage.getItem("storeUser"));

    if (user.login == login || user.email == email) {
        console.log("FALSE");
        return false;
    }
    else {        
        console.log("TRUE");
        return true;
    }
}

/**
 * Hide login form, show register form
 */
function createAccountClick() {
    document.getElementsByClassName("register-form")[0].style.display = "block";
    document.getElementsByClassName("login-form")[0].style.display = "none";
}

/**
 * Hide register form, show login form
 */
function signInClick() {
    document.getElementsByClassName("register-form")[0].style.display = "none";
    document.getElementsByClassName("login-form")[0].style.display = "block";
}
