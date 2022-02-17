/**
 * ITU project 
 * Author: Martina Chripková (xchrip01)
 */
function displayUserInfo() {
    var user = JSON.parse(localStorage.getItem("storeUser"));

    document.getElementsByClassName("user")[0].innerHTML = `
    <h2>${user.login}</h2>
    <h4>${user.email}</h4>
    `
}

function checkPwd(form) {

    var user = JSON.parse(localStorage.getItem("storeUser"));
    var user_pwd = user.password;

    var pwd = document.forms["change-pwd"]["pwd"].value;
    var pwd1 = document.forms["change-pwd"]["pwd1"].value;
    var pwd2 = document.forms["change-pwd"]["pwd2"].value;

    if (pwd1 == "" && pwd2 == "" && pwd == "")
        return true;
    else if (pwd != user_pwd) {
        alert("Incorrect password");
        return false;
    }
    else if (pwd1 != pwd2){
        alert("Password did not match");
        return false;
    }
    else{
        alert("Your password has been changed successfully!");
        user.password = pwd1;
        return true;
    }
}