include $(TOPDIR)/rules.mk

PKG_NAME:=commandlistener
PKG_RELEASE:=2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
	
define Package/commandlistener
  SECTION:=MyMANET
  CATEGORY:=MyMANET
  TITLE:=GNSM commandlistener module for MyMANET
endef

define Package/commandlistener/description
 This contains executable named commandlistener, which is a part of MyMANET GNSM.
endef

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/commandlistener/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/commandlistener $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,commandlistener))
