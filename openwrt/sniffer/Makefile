include $(TOPDIR)/rules.mk

PKG_NAME:=sniffer
PKG_RELEASE:=2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
	
define Package/sniffer
  SECTION:=MyMANET
  CATEGORY:=MyMANET
  TITLE:=GNSM sniffer module for MyMANET
endef

define Package/sniffer/description
 This contains executable named sniff, which is a part of MyMANET GNSM.
endef

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/sniffer/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sniff $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,sniffer))
