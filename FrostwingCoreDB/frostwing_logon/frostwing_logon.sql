/*
Navicat MySQL Data Transfer

Source Server         : Frostwing
Source Server Version : 50509
Source Host           : localhost:3306
Source Database       : ascemu_logon

Target Server Type    : MYSQL
Target Server Version : 50509
File Encoding         : 65001

Date: 2015-10-02 19:31:33
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `accounts`
-- ----------------------------
DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts` (
  `acct` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT 'Unique ID',
  `login` varchar(32) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Login username',
  `password` varchar(32) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Login password',
  `encrypted_password` varchar(42) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `gm` varchar(32) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Game permissions',
  `banned` int(10) unsigned NOT NULL,
  `lastlogin` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00' COMMENT 'Last login timestamp',
  `lastip` varchar(16) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Last remote address',
  `email` varchar(64) COLLATE utf8_unicode_ci NOT NULL DEFAULT '' COMMENT 'Contact e-mail address',
  `flags` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Client flags',
  `forceLanguage` varchar(5) COLLATE utf8_unicode_ci NOT NULL DEFAULT 'enUS',
  `muted` int(30) NOT NULL DEFAULT '0',
  `banreason` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  `joindate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`acct`),
  UNIQUE KEY `a` (`login`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='Account Information';

-- ----------------------------
-- Records of accounts
-- ----------------------------
INSERT INTO `accounts` VALUES ('1', 'admin', '', '8301316d0d8448a34fa6d0c6bf1cbfa2b4a1a93a', 'az', '0', '0000-00-00 00:00:00', '', 'cronic@crondevrepack.com', '24', 'enUS', '0', '', '2015-09-10 16:06:05');

-- ----------------------------
-- Table structure for `ipbans`
-- ----------------------------
DROP TABLE IF EXISTS `ipbans`;
CREATE TABLE `ipbans` (
  `ip` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `expire` int(10) NOT NULL COMMENT 'Expiry time (s)',
  `banreason` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`ip`),
  UNIQUE KEY `a` (`ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci COMMENT='IPBanner';

-- ----------------------------
-- Records of ipbans
-- ----------------------------
