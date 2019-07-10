-- phpMyAdmin SQL Dump
-- version 4.8.4
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: 19-Jun-2019 às 06:47
-- Versão do servidor: 10.1.37-MariaDB
-- versão do PHP: 7.3.0

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `controledeacesso`
--
CREATE DATABASE IF NOT EXISTS `controledeacesso` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `controledeacesso`;

-- --------------------------------------------------------

--
-- Estrutura da tabela `tb_historico`
--

CREATE TABLE `tb_historico` ( 
  `id_historico` int(11) NOT NULL AUTO_INCREMENT,
  `dataAcao_historico` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP, 
  `idMorador_historico` int(11) NOT NULL, 
  PRIMARY KEY (`id_historico`) 
); ENGINE=InnoDB DEFAULT CHARSET=latin1


-- --------------------------------------------------------

--
-- Estrutura da tabela `tb_morador`
--

CREATE TABLE `tb_morador` (
  `id_morador` int(11) NOT NULL AUTO_INCREMENT,
  `nome_morador` varchar(30) NOT NULL,
  `idCartao_morador` char(11) NOT NULL,
  PRIMARY KEY (`id_morador`)

) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Extraindo dados da tabela `tb_morador`
--

INSERT INTO `tb_morador` (`nome_morador`, `idCartao_morador`) VALUES
  ('Matheus Ferreira', 'D3 D3 65 F6'), 
  ("Jecé Xavier", "C5 15 B1 4F"),
  ("Israel Rodrigues", "96 1A D6 9D");


-- --------------------------------------------------------
--
-- ADD FOREIGN KEY
-- 

ALTER TABLE tb_historico
	ADD CONSTRAINT FK_Morador
		FOREIGN KEY (idMorador_historico) REFERENCES tb_morador(id_morador); 
