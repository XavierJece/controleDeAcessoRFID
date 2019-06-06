<?php 

    /*Requisições = Import*/
    require_once 'Config.php';

class Conecta extends Config{
    
    
    /*Constructor */
    function __construct(){
        $this->pdo = new PDO('mysql:host=' . $this->host . ';dbname=' . $this->db, $this->user, $this->pass, 
            array(PDO::MYSQL_ATTR_INIT_COMMAND => "SET NAMES utf8"));
    }



    /*My functions*/

    function select(){
        $stmt = $this->pdo->prepare("SELECT * FROM  WHERE tipo = :a ORDER BY  `data` desc");
        $stmt->bindValue(":a", $tipo);
        $run = $stmt->execute();

        $lista = array();
        while ($rs = $stmt->fetch(PDO::FETCH_ASSOC)) {
            array_push($lista, $rs);
        }

        return $lista;
    }

    function insert(){
        $idMorador = 1;
        
        $stmt = $this->pdo->prepare("INSERT INTO `tb_historico`(`idMorador_historico`) VALUES (:idMorador)");
        $stmt->bindValue(":idMorador", $idMorador);
        $run = $stmt->execute();

    }


}
?>