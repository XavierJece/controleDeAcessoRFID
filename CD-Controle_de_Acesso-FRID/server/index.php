<?php

require_once 'Conecta.php';

    echo "ola mundoo! <hr>";


    if(isset($_GET['uid'])){
        $idcartao = $_GET['uid'];

        $con = new Conecta();

        $con->insert();
    }



?>