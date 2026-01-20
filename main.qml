import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

ApplicationWindow {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "ServiceHub - User/Profile/Service"
    color: "#1a1a1a"

    property int currentPageIndex: 0
    property var pageNames: [
        "📊 Dashboard",
        "👤 User",
        "📄 Profile",
        "Service",
        "Catalog",
        "Request",
        "Message",
        "Review",
        "Subscription",
        "Favorites"
    ]

    function pageNeedsAuth(index) {
        return index === 2 || index === 8   // 2 = Profile, 8 = Subscription
    }


    function gotoPage(index) {
        if (pageNeedsAuth(index) && !dataManager.loggedIn) {
            currentPageIndex = 1
            return
        }
        currentPageIndex = index
    }

    // ======= Local "storage" for services (UI-model) =======
    ListModel {
        id: serviceModel
        ListElement {
            sid: "00000000-0000-0000-0000-000000000001"
            providerId: "00000000-0000-0000-0000-0000000000AA"
            title: "Web Design"
            description: "Landing page + responsive."
            category: "Design"
            price: 50000
            active: true
            rating: 4.8
            mediaCsv: "cover.png;portfolio1.png"
            createdAtIso: "2026-01-01T12:00:00"
        }
        ListElement {
            sid: "00000000-0000-0000-0000-000000000002"
            providerId: "00000000-0000-0000-0000-0000000000BB"
            title: "App Development"
            description: "Mobile app MVP."
            category: "Development"
            price: 100000
            active: true
            rating: 4.9
            mediaCsv: "app.png"
            createdAtIso: "2026-01-02T12:00:00"
        }
    }

    QtObject {
        id: catalog

        property var _categories: ([])
        property var _searchHistory: ([])

        // ===== helpers =====
        function _trim(s) { return (s === undefined || s === null) ? "" : String(s).trim() }
        function _lower(s) { return _trim(s).toLowerCase() }

        function _indexOfId(id) {
            var sid = _trim(id)
            if (sid.length === 0) return -1
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                if (_trim(s.sid) === sid) return i
            }
            return -1
        }

        function _ensureCategory(category) {
            var c = _trim(category)
            if (c.length === 0) return
            if (_categories.indexOf(c) === -1) _categories.push(c)
        }

        function rebuildCategoriesFromServices() {
            _categories = []
            for (var i = 0; i < serviceModel.count; ++i)
                _ensureCategory(serviceModel.get(i).category)
        }

        function _cloneService(s) {
            return {
                sid: _trim(s.sid),
                providerId: _trim(s.providerId),
                title: _trim(s.title),
                description: _trim(s.description),
                category: _trim(s.category),
                price: normalizePrice(s.price),
                active: !!s.active,
                rating: clampRating(s.rating),
                mediaCsv: _trim(s.mediaCsv),
                createdAtIso: _trim(s.createdAtIso)
            }
        }

        function _serviceToJsonObj(s) {
            return {
                id: _trim(s.sid),
                providerId: _trim(s.providerId),
                title: _trim(s.title),
                description: _trim(s.description),
                category: _trim(s.category),
                price: normalizePrice(s.price),
                active: !!s.active,
                rating: clampRating(s.rating),
                createdAt: _trim(s.createdAtIso),
                media: mediaListFromCsv(_trim(s.mediaCsv))
            }
        }

        function _serviceFromJsonObj(obj) {
            function safeStr(x) { return (x === undefined || x === null) ? "" : String(x) }
            function safeBool(x) { return !!x }
            function safeNum(x) {
                var n = Number(x)
                if (isNaN(n)) n = 0
                return n
            }

            var mediaArr = (obj.media && obj.media.length) ? obj.media : []
            var csv = mediaCsvFromList(mediaArr.map(function(it){ return safeStr(it) }))

            return {
                sid: safeStr(obj.id),
                providerId: safeStr(obj.providerId),
                title: safeStr(obj.title),
                description: safeStr(obj.description),
                category: safeStr(obj.category),
                price: normalizePrice(safeNum(obj.price)),
                active: safeBool(obj.active),
                rating: clampRating(safeNum(obj.rating)),
                mediaCsv: csv,
                createdAtIso: safeStr(obj.createdAt)
            }
        }

        function _allServicesArray() {
            var arr = []
            for (var i = 0; i < serviceModel.count; ++i)
                arr.push(_cloneService(serviceModel.get(i)))
            return arr
        }

        // ===== API =====
        function addService(serviceObj) {
            var s = _cloneService(serviceObj)
            if (_trim(s.sid).length === 0) return false

            var idx = _indexOfId(s.sid)
            if (idx >= 0) {
                serviceModel.setProperty(idx, "providerId", s.providerId)
                serviceModel.setProperty(idx, "title", s.title)
                serviceModel.setProperty(idx, "description", s.description)
                serviceModel.setProperty(idx, "category", s.category)
                serviceModel.setProperty(idx, "price", s.price)
                serviceModel.setProperty(idx, "active", s.active)
                serviceModel.setProperty(idx, "rating", s.rating)
                serviceModel.setProperty(idx, "mediaCsv", s.mediaCsv)
                serviceModel.setProperty(idx, "createdAtIso", s.createdAtIso)
            } else {
                serviceModel.append(s)
            }

            _ensureCategory(s.category)
            return true
        }

        function removeService(serviceId) {
            var idx = _indexOfId(serviceId)
            if (idx < 0) return false
            serviceModel.remove(idx)
            catalog.rebuildCategoriesFromServices()
            return true
        }

        function updateService(serviceObj) {
            var s = _cloneService(serviceObj)
            var idx = _indexOfId(s.sid)
            if (idx < 0) return false

            serviceModel.setProperty(idx, "providerId", s.providerId)
            serviceModel.setProperty(idx, "title", s.title)
            serviceModel.setProperty(idx, "description", s.description)
            serviceModel.setProperty(idx, "category", s.category)
            serviceModel.setProperty(idx, "price", s.price)
            serviceModel.setProperty(idx, "active", s.active)
            serviceModel.setProperty(idx, "rating", s.rating)
            serviceModel.setProperty(idx, "mediaCsv", s.mediaCsv)
            serviceModel.setProperty(idx, "createdAtIso", s.createdAtIso)

            _ensureCategory(s.category)
            return true
        }

        function searchByName(name) {
            var q = _trim(name)
            if (q.length === 0) return []
            var out = []
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                if (_lower(s.title).indexOf(q.toLowerCase()) !== -1)
                    out.push(_cloneService(s))
            }
            return out
        }

        function searchByDescription(text) {
            var q = _trim(text)
            if (q.length === 0) return []
            var out = []
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                if (_lower(s.description).indexOf(q.toLowerCase()) !== -1)
                    out.push(_cloneService(s))
            }
            return out
        }

        function filterByCategory(category) {
            var c = _trim(category)
            if (c.length === 0) return []
            var out = []
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                if (_trim(s.category) === c)
                    out.push(_cloneService(s))
            }
            return out
        }

        function filterByPrice(minPrice, maxPrice) {
            var minP = Number(minPrice)
            var maxP = Number(maxPrice)
            if (isNaN(minP)) minP = 0
            if (isNaN(maxP)) maxP = 0
            if (minP > maxP) return []

            var out = []
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                var p = Number(s.price)
                if (isNaN(p)) p = 0
                if (p >= minP && p <= maxP) out.push(_cloneService(s))
            }
            return out
        }

        function filterByRating(minRating) {
            var r0 = Number(minRating)
            if (isNaN(r0)) r0 = 0
            var out = []
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                var r = Number(s.rating)
                if (isNaN(r)) r = 0
                if (r >= r0) out.push(_cloneService(s))
            }
            return out
        }

        function getActiveServices() {
            var out = []
            for (var i = 0; i < serviceModel.count; ++i) {
                var s = serviceModel.get(i)
                if (!!s.active) out.push(_cloneService(s))
            }
            return out
        }

        function getPopularServices(count) {
            var n = Number(count)
            if (isNaN(n) || n < 0) n = 0

            var sorted = _allServicesArray()
            for (var i = 0; i < sorted.length - 1; ++i) {
                for (var j = i + 1; j < sorted.length; ++j) {
                    if (Number(sorted[i].rating) < Number(sorted[j].rating)) {
                        var tmp = sorted[i]; sorted[i] = sorted[j]; sorted[j] = tmp
                    }
                }
            }
            if (sorted.length > n) sorted = sorted.slice(0, n)
            return sorted
        }

        function getNewServices(count) {
            var n = Number(count)
            if (isNaN(n) || n < 0) n = 0

            var sorted = _allServicesArray()
            function dt(iso) {
                var d = Date.parse(_trim(iso))
                return isNaN(d) ? 0 : d
            }

            for (var i = 0; i < sorted.length - 1; ++i) {
                for (var j = i + 1; j < sorted.length; ++j) {
                    if (dt(sorted[i].createdAtIso) < dt(sorted[j].createdAtIso)) {
                        var tmp = sorted[i]; sorted[i] = sorted[j]; sorted[j] = tmp
                    }
                }
            }
            if (sorted.length > n) sorted = sorted.slice(0, n)
            return sorted
        }

        function getAllServices() { return _allServicesArray() }
        function getCategories() { return _categories.slice(0) }
        function getSearchHistory() { return _searchHistory.slice(0) }

        function addSearchHistory(query) {
            var q = _trim(query)
            if (q.length === 0) return
            var out = [q]
            for (var i = 0; i < _searchHistory.length; ++i) {
                if (_searchHistory[i] !== q) out.push(_searchHistory[i])
                if (out.length >= 50) break
            }
            _searchHistory = out
        }

        function getInfo() {
            return "Каталог: " + serviceModel.count + " услуг в " + _categories.length + " категориях"
        }

        function getFullInfo() {
            return "=== КАТАЛОГ ===\n"
                 + "Услуг: " + serviceModel.count + "\n"
                 + "Категорий: " + _categories.length + "\n"
                 + "История поиска: " + _searchHistory.length
        }

        function toJson() {
            var servicesArray = []
            for (var i = 0; i < serviceModel.count; ++i) {
                servicesArray.push(_serviceToJsonObj(serviceModel.get(i)))
            }
            return {
                services: servicesArray,
                categories: _categories.slice(0),
                searchHistory: _searchHistory.slice(0)
            }
        }

        function fromJson(obj) {
            if (!obj) return false

            _categories = []
            if (obj.categories && obj.categories.length) {
                for (var i = 0; i < obj.categories.length; ++i) {
                    var c = _trim(obj.categories[i])
                    if (c.length) _categories.push(c)
                }
            }

            _searchHistory = []
            if (obj.searchHistory && obj.searchHistory.length) {
                for (var j = 0; j < obj.searchHistory.length; ++j) {
                    var h = _trim(obj.searchHistory[j])
                    if (h.length) _searchHistory.push(h)
                }
            }

            serviceModel.clear()
            if (obj.services && obj.services.length) {
                for (var k = 0; k < obj.services.length; ++k) {
                    var s = _serviceFromJsonObj(obj.services[k])
                    serviceModel.append(s)
                    _ensureCategory(s.category)
                }
            }
            return true
        }
    }

    // ======= Helpers matching C++ Service behavior =======
    function clampRating(r) {
        var x = Number(r)
        if (isNaN(x)) x = 0
        if (x < 0) x = 0
        if (x > 5) x = 5
        return x
    }

    function normalizePrice(p) {
        var x = Number(p)
        if (isNaN(x) || x < 0) x = 0
        return x
    }

    function mediaListFromCsv(csv) {
        var s = (csv || "").trim()
        if (s === "") return []
        return s.split(";").map(function(it){ return it.trim() }).filter(function(it){ return it.length > 0 })
    }

    function mediaCsvFromList(arr) {
        if (!arr || arr.length === 0) return ""
        return arr.join(";")
    }

    function serviceInfoFromFields(title, price, rating) {
        var t = (title && title.length) ? title : "Untitled"
        return t + " | " + Number(price).toFixed(2) + " ₽ | " + clampRating(rating).toFixed(1) + "★"
    }

    function serviceFullInfoFromFields(sid, providerId, title, category, price, active, rating, mediaCsv, createdAtIso, description) {
        return "Service:\n"
             + "  id: " + (sid || "") + "\n"
             + "  providerId: " + (providerId || "") + "\n"
             + "  title: " + (title || "") + "\n"
             + "  category: " + (category || "") + "\n"
             + "  price: " + normalizePrice(price).toFixed(2) + "\n"
             + "  active: " + (active ? "true" : "false") + "\n"
             + "  rating: " + clampRating(rating).toFixed(2) + "\n"
             + "  media: " + mediaListFromCsv(mediaCsv).join(", ") + "\n"
             + "  createdAt: " + (createdAtIso || "") + "\n"
             + "  description: " + (description || "") + "\n"
    }

    function serviceToJsonObject(fields) {
        return {
            id: fields.sid,
            providerId: fields.providerId,
            title: fields.title,
            description: fields.description,
            category: fields.category,
            price: normalizePrice(fields.price),
            active: !!fields.active,
            rating: clampRating(fields.rating),
            createdAt: fields.createdAtIso,
            media: mediaListFromCsv(fields.mediaCsv)
        }
    }

    function serviceFromJsonText(jsonText) {
        var obj
        try {
            obj = JSON.parse(jsonText)
        } catch (e) {
            serviceLog("JSON parse error: " + e)
            return
        }

        function safeStr(x) { return (x === undefined || x === null) ? "" : String(x) }
        function safeBool(x) { return !!x }
        function safeNum(x) {
            var n = Number(x)
            if (isNaN(n)) n = 0
            return n
        }

        var mediaArr = (obj.media && obj.media.length) ? obj.media : []
        var csv = mediaCsvFromList(mediaArr.map(function(it){ return safeStr(it) }))

        serviceModel.append({
            sid: safeStr(obj.id),
            providerId: safeStr(obj.providerId),
            title: safeStr(obj.title),
            description: safeStr(obj.description),
            category: safeStr(obj.category),
            price: normalizePrice(safeNum(obj.price)),
            active: safeBool(obj.active),
            rating: clampRating(safeNum(obj.rating)),
            mediaCsv: csv,
            createdAtIso: safeStr(obj.createdAt)
        })

        servicePage.currentServiceIndex = serviceModel.count - 1
        servicePage.loadServiceToEditor(servicePage.currentServiceIndex)
        serviceLog("Imported service from JSON.")
    }

    function serviceLog(msg) {
        serviceLogArea.text = (msg + "\n") + serviceLogArea.text
    }

    // ======= UI theme helpers =======
    component Card: Rectangle {
        color: "#282c34"
        radius: 10
        border.color: "#2196F3"
        border.width: 1
    }

    component FieldLabel: Text {
        color: "#61dafb"
        font.pixelSize: 12
        font.bold: true
        elide: Text.ElideRight
    }

    component Field: TextField {
        color: "white"
        placeholderTextColor: "#9aa7b5"
        font.pixelSize: 12
        background: Rectangle {
            color: "#1a1a1a"
            radius: 6
            border.color: "#2196F3"
            border.width: 1
        }
    }

    component PrimaryBtn: Button {
        background: Rectangle {
            color: "#2196F3"
            radius: 6
            border.color: "#61dafb"
            border.width: 1
        }
        contentItem: Text {
            text: parent.text
            color: "white"
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    component DangerBtn: Button {
        background: Rectangle {
            color: "#d32f2f"
            radius: 6
            border.color: "#ff5252"
            border.width: 1
        }
        contentItem: Text {
            text: parent.text
            color: "white"
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    component CardPane: Pane {
        Layout.fillWidth: true
        padding: 14
        background: Rectangle {
            color: "#282c34"
            radius: 10
            border.color: "#2196F3"
            border.width: 2
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // ======= LEFT NAV =======
        Rectangle {
            Layout.preferredWidth: 240
            Layout.fillHeight: true
            color: "#1e1e1e"
            border.color: "#2196F3"
            border.width: 2
            radius: 12

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: "#2196F3"
                    radius: 8
                    Text {
                        anchors.centerIn: parent
                        text: "🚀 SERVICEHUB"
                        color: "white"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 2; color: "#2196F3" }

                ScrollView {
                    id: navSv
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ColumnLayout {
                        width: parent.width - 15
                        spacing: 8

                        Repeater {
                            model: mainWindow.pageNames.length
                            Rectangle {
                                required property int index
                                Layout.fillWidth: true
                                Layout.preferredHeight: 55
                                radius: 8
                                clip: true
                                color: mainWindow.currentPageIndex === index ? "#2196F3" : "#282c34"
                                border.color: mainWindow.currentPageIndex === index ? "#00BCD4" : "#444"
                                border.width: 2

                                Text {
                                    anchors.centerIn: parent
                                    text: mainWindow.pageNames[parent.index]
                                    color: mainWindow.currentPageIndex === parent.index ? "white" : "#61dafb"
                                    font.bold: true
                                    font.pixelSize: 13
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: gotoPage(parent.index)
                                    onEntered: if (mainWindow.currentPageIndex !== parent.index) parent.color = "#333333"
                                    onExited:  if (mainWindow.currentPageIndex !== parent.index) parent.color = "#282c34"
                                }
                            }
                        }
                    }
                }

                Rectangle { Layout.fillWidth: true; height: 1; color: "#444" }


                DangerBtn {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 45
                    text: "🚪 Logout"
                    enabled: dataManager.loggedIn
                    onClicked: {
                        dataManager.logout()
                        gotoPage(1)
                    }
                }
            }
        }

        // ======= MAIN CONTENT =======
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1a1a1a"
            border.color: "#2196F3"
            border.width: 2
            radius: 12
            clip: true

            StackLayout {
                id: pages
                anchors.fill: parent
                anchors.margins: 15
                currentIndex: mainWindow.currentPageIndex

                // PAGE 0: DASHBOARD
                Item {
                    ScrollView {
                        id: dashSv
                        anchors.fill: parent
                        clip: true
                        contentWidth: dashSv.availableWidth
                        contentHeight: dashCol.implicitHeight

                        ColumnLayout {
                            id: dashCol
                            width: dashSv.availableWidth
                            spacing: 14

                            Text { text: "📊 Dashboard"; color: "#61dafb"; font.bold: true; font.pixelSize: 26 }
                            Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                            CardPane {
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 10

                                    Text { text: "Status"; color: "#61dafb"; font.bold: true; font.pixelSize: 14 }

                                    Text {
                                        Layout.fillWidth: true
                                        text: dataManager.loggedIn ? ("Logged in: " + dataManager.currentUserEmail) : "Not logged in"
                                        color: "white"
                                        font.pixelSize: 12
                                        wrapMode: Text.WordWrap
                                    }

                                    Button {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 42
                                        text: "Go to User →"
                                        background: Rectangle { color: "#333333"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#61dafb"
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        onClicked: gotoPage(1)
                                    }
                                }
                            }

                            Item { Layout.fillHeight: true }
                        }
                    }
                }

                // PAGE 1: USER
                Item {
                    id: userPage
                    property string uiInfo: ""
                    property string uiError: ""
                    property string lastIssuedCode: ""

                    function setInfo(msg) { uiInfo = msg; uiError = "" }
                    function setError(msg) { uiError = msg; uiInfo = "" }

                    ScrollView {
                        id: userSv
                        anchors.fill: parent
                        clip: true
                        contentWidth: userSv.availableWidth
                        contentHeight: userCol.implicitHeight

                        ColumnLayout {
                            id: userCol
                            width: userSv.availableWidth
                            spacing: 14

                            Text { text: "👤 User"; color: "#61dafb"; font.bold: true; font.pixelSize: 24 }
                            Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: (userPage.uiInfo !== "" || userPage.uiError !== "") ? 44 : 0
                                visible: (userPage.uiInfo !== "" || userPage.uiError !== "")
                                radius: 8
                                color: (userPage.uiError !== "") ? "#3a1f1f" : "#1f2b22"
                                border.color: (userPage.uiError !== "") ? "#ff5252" : "#4CAF50"
                                border.width: 1
                                Text {
                                    anchors.centerIn: parent
                                    text: userPage.uiError !== "" ? userPage.uiError : userPage.uiInfo
                                    color: "white"
                                    font.pixelSize: 12
                                    font.bold: true
                                    elide: Text.ElideRight
                                }
                            }

                            // NOT LOGGED IN
                            CardPane {
                                visible: !dataManager.loggedIn

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 12

                                    Text { text: "Authorization"; color: "#61dafb"; font.bold: true; font.pixelSize: 14 }

                                    TabBar {
                                        id: authTabs
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 40
                                        TabButton { text: "Login" }
                                        TabButton { text: "Register" }
                                    }

                                    StackLayout {
                                        Layout.fillWidth: true
                                        currentIndex: authTabs.currentIndex

                                        // LOGIN
                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 10

                                            Field { id: loginEmail; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Email (or phone)" }
                                            Field { id: loginPass;  Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Password"; echoMode: TextInput.Password }

                                            PrimaryBtn {
                                                text: "Login"
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 42
                                                onClicked: {
                                                    userPage.setInfo("")
                                                    userPage.setError("")
                                                    var ok = dataManager.login(loginEmail.text, loginPass.text)
                                                    if (ok) {
                                                        userPage.setInfo("Logged in successfully")
                                                        gotoPage(0)
                                                    } else {
                                                        userPage.setError("Login failed: wrong credentials")
                                                    }
                                                }
                                            }
                                        }


                                        // REGISTER
                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 10

                                            Field { id: regEmail; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Email" }
                                            Field { id: regPhone; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Phone" }

                                            ComboBox {
                                                id: regRole
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 42
                                                model: ["Client", "Provider", "Admin"]
                                                background: Rectangle { color: "#1a1a1a"; border.color: "#2196F3"; border.width: 1; radius: 6 }
                                                contentItem: Text {
                                                    text: regRole.currentText
                                                    color: "white"
                                                    font.pixelSize: 12
                                                    verticalAlignment: Text.AlignVCenter
                                                    elide: Text.ElideRight
                                                }
                                            }

                                            Field { id: regPass; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Password (min 6)"; echoMode: TextInput.Password }

                                            Button {
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 42
                                                text: "Create account"
                                                background: Rectangle { color: "#4CAF50"; border.color: "#81C784"; border.width: 1; radius: 6 }
                                                contentItem: Text {
                                                    text: parent.text
                                                    color: "white"
                                                    font.bold: true
                                                    horizontalAlignment: Text.AlignHCenter
                                                    verticalAlignment: Text.AlignVCenter
                                                }
                                                onClicked: {
                                                    userPage.setInfo("")
                                                    userPage.setError("")
                                                    var ok = dataManager.registerUser(regEmail.text, regPhone.text, regRole.currentIndex, regPass.text)
                                                    if (ok) { userPage.setInfo("Account created and logged in"); gotoPage(2) }
                                                    else userPage.setError("Registration failed (email exists or invalid data)")
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            // LOGGED IN
                            CardPane {
                                visible: dataManager.loggedIn

                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 10

                                    Text { text: "Current user"; color: "#61dafb"; font.bold: true; font.pixelSize: 14 }

                                    Repeater {
                                        model: [
                                            { l: "userId:", v: dataManager.currentUserId, c: "white" },
                                            { l: "email:",  v: dataManager.currentUserEmail, c: "white" },
                                            { l: "phone:",  v: dataManager.currentUserPhone, c: "white" },
                                            { l: "role:",   v: dataManager.currentUserRole, c: dataManager.currentUserRole === "Admin" ? "#FF9800" : (dataManager.currentUserRole === "Provider" ? "#4CAF50" : "white") },
                                            { l: "verified:", v: (dataManager.currentUserVerified ? "true" : "false"), c: dataManager.currentUserVerified ? "#4CAF50" : "#FF9800" }
                                        ]

                                        Rectangle {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 44
                                            color: "#1a1a1a"
                                            border.color: "#2196F3"
                                            border.width: 1
                                            radius: 6

                                            RowLayout {
                                                anchors.fill: parent
                                                anchors.margins: 12
                                                spacing: 10
                                                Text { text: modelData.l; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                                Text { text: modelData.v; color: modelData.c; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                                            }
                                        }
                                    }

                                    Rectangle { Layout.fillWidth: true; height: 1; color: "#333333" }

                                    Text { text: "Verification"; color: "#61dafb"; font.bold: true; font.pixelSize: 13 }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 10

                                        Button {
                                            Layout.preferredWidth: 220
                                            Layout.preferredHeight: 42
                                            text: "Issue verification code"
                                            background: Rectangle { color: "#00BCD4"; radius: 6 }
                                            contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                            onClicked: {
                                                userPage.setInfo("")
                                                userPage.setError("")
                                                var code = dataManager.issueVerificationCode()
                                                if (code === "") userPage.setError("Cannot issue code")
                                                else { userPage.lastIssuedCode = code; userPage.setInfo("Code issued (demo): " + code) }
                                            }
                                        }

                                        Text {
                                            Layout.fillWidth: true
                                            text: userPage.lastIssuedCode !== "" ? ("Last code: " + userPage.lastIssuedCode) : ""
                                            color: "#CE93D8"
                                            font.pixelSize: 12
                                            elide: Text.ElideRight
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 10
                                        Field { id: verifyCode; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Enter verification code" }

                                        Button {
                                            Layout.preferredWidth: 120
                                            Layout.preferredHeight: 42
                                            text: "Verify"
                                            background: Rectangle { color: "#4CAF50"; radius: 6 }
                                            contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                            onClicked: {
                                                userPage.setInfo("")
                                                userPage.setError("")
                                                var ok = dataManager.verifyAccount(verifyCode.text)
                                                if (ok) userPage.setInfo("Account verified")
                                                else userPage.setError("Verification failed")
                                            }
                                        }
                                    }

                                    Rectangle { Layout.fillWidth: true; height: 1; color: "#333333" }

                                    Text { text: "Change password"; color: "#61dafb"; font.bold: true; font.pixelSize: 13 }
                                    Field { id: oldPass; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Old password"; echoMode: TextInput.Password }
                                    Field { id: newPass; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "New password (min 6)"; echoMode: TextInput.Password }

                                    PrimaryBtn {
                                        text: "Update password"
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 42
                                        onClicked: {
                                            userPage.setInfo("")
                                            userPage.setError("")
                                            var ok = dataManager.changePassword(oldPass.text, newPass.text)
                                            if (ok) userPage.setInfo("Password changed")
                                            else userPage.setError("Password change failed")
                                        }
                                    }

                                    Button {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 42
                                        text: "Go to Profile →"
                                        background: Rectangle { color: "#333333"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#61dafb"
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        onClicked: gotoPage(2)
                                    }
                                }
                            }

                            Item { Layout.fillHeight: true }
                        }
                    }
                }

                // PAGE 2: PROFILE
                Item {
                    id: profilePage

                    property string uiInfo: ""
                    property string uiError: ""

                    function loadProfileToUi() {
                        uiInfo = ""; uiError = ""
                        if (!dataManager.loggedIn) { uiError = "Login first"; return }
                        var p = dataManager.getMyProfile()
                        profileId.text = p.profileId || ""
                        ownerId.text = p.ownerUserId || ""
                        nameField.text = p.name || ""
                        descArea.text = p.description || ""
                        avatarField.text = p.avatarPath || ""
                        emailField.text = p.contactEmail || ""
                        phoneField.text = p.contactPhone || ""
                    }

                    Connections {
                        target: dataManager
                        function onLoggedInChanged() { profilePage.loadProfileToUi() }
                    }
                    Component.onCompleted: loadProfileToUi()

                    ScrollView {
                        id: profSv
                        anchors.fill: parent
                        clip: true
                        contentWidth: profSv.availableWidth
                        contentHeight: profCol.implicitHeight

                        ColumnLayout {
                            id: profCol
                            width: profSv.availableWidth
                            spacing: 14

                            Text { text: "📄 Profile"; color: "#61dafb"; font.bold: true; font.pixelSize: 24 }
                            Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: (uiInfo !== "" || uiError !== "") ? 44 : 0
                                visible: (uiInfo !== "" || uiError !== "")
                                radius: 8
                                color: (uiError !== "") ? "#3a1f1f" : "#1f2b22"
                                border.color: (uiError !== "") ? "#ff5252" : "#4CAF50"
                                border.width: 1
                                Text {
                                    anchors.centerIn: parent
                                    text: uiError !== "" ? uiError : uiInfo
                                    color: "white"
                                    font.pixelSize: 12
                                    font.bold: true
                                }
                            }

                            CardPane {
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 10

                                    Text { text: "profileId:"; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                    Field { id: profileId; Layout.fillWidth: true; Layout.preferredHeight: 42; readOnly: true }

                                    Text { text: "userId (owner):"; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                    Field { id: ownerId; Layout.fillWidth: true; Layout.preferredHeight: 42; readOnly: true }

                                    Text { text: "name:"; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                    Field { id: nameField; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "Company / Name" }

                                    Text { text: "description:"; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                    TextArea {
                                        id: descArea
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 140
                                        color: "white"
                                        placeholderText: "Описание профиля..."
                                        placeholderTextColor: "#9aa7b5"
                                        wrapMode: TextArea.Wrap
                                        background: Rectangle {
                                            color: "#1a1a1a"
                                            border.color: "#2196F3"
                                            border.width: 1
                                            radius: 6
                                        }
                                    }

                                    Text { text: "avatarPath:"; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                    Field { id: avatarField; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "ALDA.png" }

                                    Text { text: "contacts:"; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 10

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6
                                            Text { text: "email:"; color: "#9aa7b5"; font.pixelSize: 11 }
                                            Field { id: emailField; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "mail@example.com" }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6
                                            Text { text: "phone:"; color: "#9aa7b5"; font.pixelSize: 11 }
                                            Field { id: phoneField; Layout.fillWidth: true; Layout.preferredHeight: 42; placeholderText: "+7..." }
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 10

                                        Button {
                                            Layout.preferredWidth: 180
                                            Layout.preferredHeight: 42
                                            text: "Reload"
                                            background: Rectangle { color: "#333333"; radius: 6 }
                                            contentItem: Text {
                                                text: parent.text
                                                color: "white"
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                            onClicked: loadProfileToUi()
                                        }

                                        Button {
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 42
                                            text: "Save profile"
                                            background: Rectangle { color: "#4CAF50"; radius: 6 }
                                            contentItem: Text {
                                                text: parent.text
                                                color: "white"
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                            onClicked: {
                                                uiInfo = ""; uiError = ""
                                                if (!dataManager.loggedIn) { uiError = "Login first"; return }
                                                var ok = dataManager.saveMyProfile(
                                                            nameField.text,
                                                            descArea.text,
                                                            avatarField.text,
                                                            emailField.text,
                                                            phoneField.text)
                                                if (ok) {
                                                    uiInfo = "Profile saved"
                                                    loadProfileToUi()
                                                } else {
                                                    uiError = "Save failed"
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            Item { Layout.fillHeight: true }
                        }
                    }
                }

                // PAGE 3: SERVICE
                Item {
                    id: servicePage
                    property int currentServiceIndex: -1

                    function loadServiceToEditor(index) {
                        if (index < 0 || index >= serviceModel.count) return
                        var s = serviceModel.get(index)
                        sidField.text = s.sid
                        providerIdField.text = s.providerId
                        titleField.text = s.title
                        categoryField.text = s.category
                        priceField.text = String(s.price)
                        activeSwitch.checked = !!s.active
                        ratingField.text = String(s.rating)
                        createdAtField.text = s.createdAtIso
                        descriptionArea.text = s.description
                        mediaCsvField.text = s.mediaCsv
                        jsonArea.text = ""
                        infoArea.text = serviceInfoFromFields(titleField.text, priceField.text, ratingField.text)
                        fullInfoArea.text = serviceFullInfoFromFields(
                                    sidField.text, providerIdField.text, titleField.text, categoryField.text,
                                    priceField.text, activeSwitch.checked, ratingField.text,
                                    mediaCsvField.text, createdAtField.text, descriptionArea.text)
                    }

                    function clearEditor() {
                        sidField.text = ""
                        providerIdField.text = ""
                        titleField.text = ""
                        categoryField.text = ""
                        priceField.text = "0"
                        activeSwitch.checked = true
                        ratingField.text = "0"
                        createdAtField.text = ""
                        descriptionArea.text = ""
                        mediaCsvField.text = ""
                        jsonArea.text = ""
                        infoArea.text = ""
                        fullInfoArea.text = ""
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Text {
                                text: "Service Management"
                                color: "#61dafb"
                                font.pixelSize: 26
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            PrimaryBtn {
                                text: "Add New Service"
                                Layout.preferredWidth: 200
                                Layout.preferredHeight: 42
                                onClicked: {
                                    var id = servicePage.newUuidLike()
                                    serviceModel.append({
                                        sid: id,
                                        providerId: providerIdField.text.trim(),
                                        title: titleField.text,
                                        description: descriptionArea.text,
                                        category: categoryField.text,
                                        price: normalizePrice(priceField.text),
                                        active: activeSwitch.checked,
                                        rating: clampRating(ratingField.text),
                                        mediaCsv: mediaCsvField.text.trim(),
                                        createdAtIso: (createdAtField.text.trim() !== "") ? createdAtField.text.trim() : new Date().toISOString()
                                    })
                                    catalog.rebuildCategoriesFromServices()
                                    serviceList.currentIndex = serviceModel.count - 1
                                    serviceLog("Created new service (from editor).")
                                }
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 12

                            // ===== Left: list =====
                            Card {
                                Layout.preferredWidth: 360
                                Layout.fillHeight: true
                                border.width: 2

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 10

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8
                                        Text { text: "Services"; color: "#61dafb"; font.bold: true; font.pixelSize: 14; Layout.fillWidth: true }
                                        Text { text: "(" + serviceModel.count + ")"; color: "#888"; font.pixelSize: 12 }
                                    }

                                    ListView {
                                        id: serviceList
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        clip: true
                                        model: serviceModel
                                        spacing: 8
                                        currentIndex: servicePage.currentServiceIndex

                                        onCurrentIndexChanged: {
                                            servicePage.currentServiceIndex = currentIndex
                                            if (currentIndex >= 0) servicePage.loadServiceToEditor(currentIndex)
                                        }

                                        delegate: Rectangle {
                                            width: serviceList.width
                                            height: 80
                                            radius: 8
                                            color: (index === serviceList.currentIndex) ? "#2196F3" : "#1a1a1a"
                                            border.color: "#2196F3"
                                            border.width: 1

                                            ColumnLayout {
                                                anchors.fill: parent
                                                anchors.margins: 10
                                                spacing: 4

                                                Text {
                                                    text: (title && title.length) ? title : "Untitled"
                                                    color: "white"
                                                    font.bold: true
                                                    font.pixelSize: 13
                                                    elide: Text.ElideRight
                                                }

                                                Text {
                                                    text: (category || "") + " | " + Number(price).toFixed(0) + " ₽"
                                                    color: "#bbb"
                                                    font.pixelSize: 11
                                                    elide: Text.ElideRight
                                                }

                                                RowLayout {
                                                    Layout.fillWidth: true
                                                    spacing: 8
                                                    Text { text: clampRating(rating).toFixed(1) + "★"; color: "#FFB74D"; font.bold: true; font.pixelSize: 12 }
                                                    Rectangle {
                                                        width: 70; height: 20; radius: 4
                                                        color: active ? "#4CAF50" : "#FF9800"
                                                        Text { anchors.centerIn: parent; text: active ? "Active" : "Off"; color: "white"; font.pixelSize: 10; font.bold: true }
                                                    }
                                                    Item { Layout.fillWidth: true }
                                                }
                                            }

                                            TapHandler {
                                                onTapped: serviceList.currentIndex = index
                                            }
                                        }
                                    }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8

                                        DangerBtn {
                                            text: "Delete"
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 40
                                            enabled: serviceList.currentIndex >= 0
                                            onClicked: {
                                                var idx = serviceList.currentIndex
                                                if (idx < 0) return
                                                serviceModel.remove(idx)
                                                catalog.rebuildCategoriesFromServices()
                                                if (serviceModel.count > 0) {
                                                    serviceList.currentIndex = Math.min(idx, serviceModel.count - 1)
                                                } else {
                                                    serviceList.currentIndex = -1
                                                    servicePage.currentServiceIndex = -1
                                                    clearEditor()
                                                }
                                                serviceLog("Deleted service (UI).")
                                            }
                                        }

                                        PrimaryBtn {
                                            text: "Save"
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 40
                                            enabled: serviceList.currentIndex >= 0
                                            onClicked: {
                                                var idx = serviceList.currentIndex
                                                if (idx < 0) return

                                                serviceModel.setProperty(idx, "sid", sidField.text.trim())
                                                serviceModel.setProperty(idx, "providerId", providerIdField.text.trim())
                                                serviceModel.setProperty(idx, "title", titleField.text)
                                                serviceModel.setProperty(idx, "description", descriptionArea.text)
                                                serviceModel.setProperty(idx, "category", categoryField.text)
                                                serviceModel.setProperty(idx, "price", normalizePrice(priceField.text))
                                                serviceModel.setProperty(idx, "active", activeSwitch.checked)
                                                serviceModel.setProperty(idx, "rating", clampRating(ratingField.text))
                                                serviceModel.setProperty(idx, "mediaCsv", mediaCsvField.text.trim())
                                                serviceModel.setProperty(idx, "createdAtIso", createdAtField.text.trim())
                                                catalog.rebuildCategoriesFromServices()
                                                servicePage.loadServiceToEditor(idx)
                                                serviceLog("Saved changes (UI).")
                                            }
                                        }
                                    }
                                }
                            }

                            // ===== Right: editor =====
                            Card {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                border.width: 2

                                ScrollView {
                                    anchors.fill: parent
                                    clip: true

                                    ColumnLayout {
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.margins: 12
                                        spacing: 12

                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 10
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "Service ID (id)" }
                                                Field { id: sidField; Layout.fillWidth: true; placeholderText: "uuid"; readOnly: false }
                                            }
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "Provider ID (providerId)" }
                                                Field { id: providerIdField; Layout.fillWidth: true; placeholderText: "uuid" }
                                            }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 10
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "Title" }
                                                Field { id: titleField; Layout.fillWidth: true; placeholderText: "Title" }
                                            }
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "Category" }
                                                Field { id: categoryField; Layout.fillWidth: true; placeholderText: "Category" }
                                            }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 10
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "Price" }
                                                Field { id: priceField; Layout.fillWidth: true; placeholderText: "0" }
                                            }
                                            ColumnLayout {
                                                Layout.preferredWidth: 220
                                                spacing: 6
                                                FieldLabel { text: "Active" }
                                                Switch {
                                                    id: activeSwitch
                                                    text: activeSwitch.checked ? "true" : "false"
                                                    palette.highlight: "#4CAF50"
                                                }
                                            }
                                            ColumnLayout {
                                                Layout.preferredWidth: 220
                                                spacing: 6
                                                FieldLabel { text: "Rating (0..5)" }
                                                Field { id: ratingField; Layout.fillWidth: true; placeholderText: "0.0" }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6
                                            FieldLabel { text: "CreatedAt (ISO)" }
                                            Field { id: createdAtField; Layout.fillWidth: true; placeholderText: "2026-01-01T12:00:00" }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6
                                            FieldLabel { text: "Description" }
                                            TextArea {
                                                id: descriptionArea
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 120
                                                color: "white"
                                                wrapMode: TextArea.Wrap
                                                placeholderText: "Description..."
                                                background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 8
                                            FieldLabel { text: "Media CSV (через ; )" }
                                            Field { id: mediaCsvField; Layout.fillWidth: true; placeholderText: "a.png;b.png" }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 8
                                            PrimaryBtn {
                                                text: "getInfo()"
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 40
                                                enabled: serviceList.currentIndex >= 0
                                                onClicked: {
                                                    infoArea.text = serviceInfoFromFields(titleField.text, priceField.text, ratingField.text)
                                                    serviceLog("getInfo generated.")
                                                }
                                            }
                                            PrimaryBtn {
                                                text: "getFullInfo()"
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 40
                                                enabled: serviceList.currentIndex >= 0
                                                onClicked: {
                                                    fullInfoArea.text = serviceFullInfoFromFields(
                                                                    sidField.text, providerIdField.text, titleField.text, categoryField.text,
                                                                    priceField.text, activeSwitch.checked, ratingField.text,
                                                                    mediaCsvField.text, createdAtField.text, descriptionArea.text)
                                                    serviceLog("getFullInfo generated.")
                                                }
                                            }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 10
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "Info output" }
                                                TextArea {
                                                    id: infoArea
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: 60
                                                    readOnly: true
                                                    color: "white"
                                                    wrapMode: TextArea.Wrap
                                                    background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                                }
                                            }
                                            ColumnLayout {
                                                Layout.fillWidth: true
                                                spacing: 6
                                                FieldLabel { text: "FullInfo output" }
                                                TextArea {
                                                    id: fullInfoArea
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: 120
                                                    readOnly: true
                                                    color: "white"
                                                    wrapMode: TextArea.Wrap
                                                    background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                                }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6
                                            FieldLabel { text: "JSON area" }
                                            TextArea {
                                                id: jsonArea
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 160
                                                color: "white"
                                                wrapMode: TextArea.Wrap
                                                placeholderText: "{ ... }"
                                                background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                            }
                                        }

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 6
                                            FieldLabel { text: "Log" }
                                            TextArea {
                                                id: serviceLogArea
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 140
                                                readOnly: true
                                                color: "white"
                                                wrapMode: TextArea.Wrap
                                                background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#444"; border.width: 1 }
                                            }
                                        }

                                        Item { Layout.fillHeight: true }
                                    }
                                }
                            }
                        }
                    }

                    Component.onCompleted: {
                        if (serviceModel.count > 0) {
                            serviceList.currentIndex = 0
                            serviceLog("Service page ready.")
                        } else {
                            clearEditor()
                            serviceLog("No services in model.")
                        }
                    }

                    function newUuidLike() {
                        return "demo-" + Date.now() + "-" + Math.floor(Math.random()*100000)
                    }
                }

                // =======================
                // PAGE 4: CATALOG (with Favorites ★)
                // =======================
                Item {
                    id: catalogPage

                    ListModel { id: catalogModel }     // services shown now
                    ListModel { id: historyModel }     // search history shown at left

                    property string statusText: ""
                    property var favServiceIds: []

                    function t(x) { return (x === undefined || x === null) ? "" : String(x).trim() }

                    // -------- favorites cache --------
                    function refreshFavCache() {
                        favServiceIds = []
                        if (!dataManager.loggedIn) return
                        var f = dataManager.getMyFavorites()
                        if (f && f.favoriteServiceIds) favServiceIds = f.favoriteServiceIds
                    }

                    function isFavService(sid) {
                        var id = t(sid)
                        for (var i = 0; i < favServiceIds.length; ++i)
                            if (t(favServiceIds[i]) === id) return true
                        return false
                    }

                    // -------- existing catalog helpers --------
                    function rebuildHistoryModel() {
                        historyModel.clear()
                        var hist = catalog.getSearchHistory()
                        for (var j = 0; j < hist.length; ++j)
                            historyModel.append({ q: hist[j] })
                    }

                    function loadList(list, label) {
                        catalogModel.clear()
                        for (var i = 0; i < list.length; ++i)
                            catalogModel.append(list[i])
                        statusText = label + " (" + list.length + ")"
                        rebuildHistoryModel()
                    }

                    function reloadAll()    { loadList(catalog.getAllServices(), "All services") }
                    function reloadActive() { loadList(catalog.getActiveServices(), "Active") }
                    function reloadPopular(){ loadList(catalog.getPopularServices(10), "Popular top 10") }
                    function reloadNew()    { loadList(catalog.getNewServices(10), "New top 10") }

                    function runSearch(q) {
                        var qq = t(q)
                        catalog.addSearchHistory(qq)
                        loadList(catalog.searchByName(qq), "Search: " + qq)
                    }

                    // -------- sync --------
                    onVisibleChanged: if (visible) { reloadAll(); refreshFavCache() }

                    Connections {
                        target: serviceModel
                        function onCountChanged() { if (catalogPage.visible) catalogPage.reloadAll() }
                    }

                    Connections {
                        target: dataManager
                        function onFavoritesChanged() { if (catalogPage.visible) catalogPage.refreshFavCache() }
                        function onLoggedInChanged() { if (catalogPage.visible) catalogPage.refreshFavCache() }
                    }

                    Component.onCompleted: {
                        rebuildHistoryModel()
                        reloadAll()
                        refreshFavCache()
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 12

                            Text {
                                text: "Catalog"
                                color: "#61dafb"
                                font.pixelSize: 26
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            Text {
                                text: serviceModel.count
                                color: "white"
                                font.pixelSize: 12
                                elide: Text.ElideRight
                            }

                            PrimaryBtn {
                                text: "Reload all"
                                Layout.preferredWidth: 160
                                Layout.preferredHeight: 40
                                onClicked: catalogPage.reloadAll()
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 12

                            // ---------- LEFT PANEL ----------
                            Card {
                                Layout.preferredWidth: 360
                                Layout.fillHeight: true
                                border.width: 2

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 10

                                    FieldLabel { text: "Search by name" }

                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 8

                                        Field {
                                            id: searchField
                                            Layout.fillWidth: true
                                            Layout.preferredHeight: 40
                                            placeholderText: "Type title..."
                                            onAccepted: catalogPage.runSearch(text)
                                        }

                                        PrimaryBtn {
                                            text: "Search"
                                            Layout.preferredWidth: 110
                                            Layout.preferredHeight: 40
                                            onClicked: catalogPage.runSearch(searchField.text)
                                        }
                                    }

                                    Rectangle { Layout.fillWidth: true; height: 1; color: "#333" }

                                    FieldLabel { text: "Search history" }

                                    ListView {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        clip: true
                                        model: historyModel
                                        spacing: 8

                                        delegate: Rectangle {
                                            width: parent.width
                                            height: 34
                                            radius: 6
                                            color: "#1a1a1a"
                                            border.color: "#333"
                                            border.width: 1

                                            Text {
                                                anchors.verticalCenter: parent.verticalCenter
                                                anchors.left: parent.left
                                                anchors.leftMargin: 10
                                                text: model.q
                                                color: "#bbb"
                                                font.pixelSize: 11
                                                elide: Text.ElideRight
                                                width: parent.width - 20
                                            }

                                            TapHandler {
                                                onTapped: {
                                                    searchField.text = model.q
                                                    catalogPage.runSearch(model.q)
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            // ---------- CENTER PANEL ----------
                            Card {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                border.width: 2

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 12
                                    spacing: 10

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 44
                                        spacing: 10

                                        Text {
                                            text: statusText
                                            color: "#9aa7b5"
                                            font.pixelSize: 12
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                        }

                                        PrimaryBtn {
                                            text: "Active only"
                                            Layout.preferredWidth: 140
                                            Layout.preferredHeight: 36
                                            onClicked: catalogPage.reloadActive()
                                        }
                                        PrimaryBtn {
                                            text: "Popular top 10"
                                            Layout.preferredWidth: 160
                                            Layout.preferredHeight: 36
                                            onClicked: catalogPage.reloadPopular()
                                        }
                                        PrimaryBtn {
                                            text: "New top 10"
                                            Layout.preferredWidth: 140
                                            Layout.preferredHeight: 36
                                            onClicked: catalogPage.reloadNew()
                                        }
                                    }

                                    TextArea {
                                        id: detailsArea
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 120
                                        readOnly: true
                                        color: "white"
                                        wrapMode: TextArea.Wrap
                                        placeholderText: "Tap a service to show full info..."
                                        placeholderTextColor: "#9aa7b5"
                                        background: Rectangle {
                                            color: "#1a1a1a"
                                            radius: 6
                                            border.color: "#2196F3"
                                            border.width: 1
                                        }
                                    }

                                    ListView {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        clip: true
                                        model: catalogModel
                                        spacing: 8

                                        delegate: Rectangle {
                                            width: parent.width
                                            height: 92
                                            radius: 10
                                            color: "#1a1a1a"
                                            border.color: "#2196F3"
                                            border.width: 1

                                            ColumnLayout {
                                                anchors.fill: parent
                                                anchors.margins: 10
                                                spacing: 4

                                                RowLayout {
                                                    Layout.fillWidth: true
                                                    spacing: 8

                                                    Text {
                                                        Layout.fillWidth: true
                                                        text: title && title.length ? title : "Untitled"
                                                        color: "white"
                                                        font.bold: true
                                                        font.pixelSize: 14
                                                        elide: Text.ElideRight
                                                    }

                                                    // ★ Favorites button
                                                    Button {
                                                        id: favBtn
                                                        Layout.preferredWidth: 44
                                                        Layout.preferredHeight: 28
                                                        text: catalogPage.isFavService(sid) ? "★" : "☆"
                                                        enabled: true

                                                        background: Rectangle {
                                                            radius: 6
                                                            color: catalogPage.isFavService(sid) ? "#FFD700" : "#333333"
                                                            border.color: "#444"
                                                            border.width: 1
                                                        }

                                                        contentItem: Text {
                                                            text: favBtn.text
                                                            color: catalogPage.isFavService(sid) ? "black" : "white"
                                                            font.bold: true
                                                            horizontalAlignment: Text.AlignHCenter
                                                            verticalAlignment: Text.AlignVCenter
                                                        }

                                                        onClicked: {
                                                            if (!dataManager.loggedIn) {
                                                                detailsArea.text = "Login required to use favorites."
                                                                gotoPage(1)
                                                                return
                                                            }

                                                            var wasFav = catalogPage.isFavService(sid)
                                                            dataManager.toggleFavoriteService(sid)
                                                            catalogPage.refreshFavCache()
                                                            detailsArea.text = (wasFav ? "Removed from favorites: " : "Added to favorites: ") + sid
                                                        }
                                                    }
                                                }

                                                RowLayout {
                                                    Layout.fillWidth: true
                                                    spacing: 10

                                                    Rectangle {
                                                        width: 74
                                                        height: 22
                                                        radius: 5
                                                        color: active ? "#4CAF50" : "#FF9800"

                                                        Text {
                                                            anchors.centerIn: parent
                                                            text: active ? "Active" : "Off"
                                                            color: "white"
                                                            font.pixelSize: 10
                                                            font.bold: true
                                                        }
                                                    }

                                                    Text {
                                                        Layout.fillWidth: true
                                                        text: (category ? category : "") +
                                                              "   " + Number(price).toFixed(0) +
                                                              "   ★ " + Number(clampRating(rating)).toFixed(1)
                                                        color: "#bbb"
                                                        font.pixelSize: 11
                                                        elide: Text.ElideRight
                                                    }
                                                }

                                                Text {
                                                    Layout.fillWidth: true
                                                    text: description && description.length ? description : ""
                                                    color: "#9aa7b5"
                                                    font.pixelSize: 11
                                                    elide: Text.ElideRight
                                                }
                                            }

                                            TapHandler {
                                                onTapped: {
                                                    detailsArea.text = serviceFullInfoFromFields(
                                                                sid, providerId, title, category, price, active,
                                                                rating, mediaCsv, createdAtIso, description)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }


                // PAGE 5: REQUEST MANAGEMENT
                Item {
                    id: requestPage
                    property string uiInfo: ""
                    property string uiError: ""
                    property int currentRequestIndex: -1

                    function setInfo(msg) { uiInfo = msg; uiError = "" }
                    function setError(msg) { uiError = msg; uiInfo = "" }

                    function clearEditor() {
                        reqIdField.text = ""
                        reqClientIdField.text = ""
                        reqProviderIdField.text = ""
                        reqServiceIdField.text = ""
                        reqDescArea.text = ""
                        reqStatusCombo.currentIndex = 0
                        reqCreatedAtField.text = ""
                        reqCompletedAtField.text = ""
                        reqCommentField.text = ""
                        reqCommentsView.text = ""
                    }

                    function loadRequestToEditor(index) {
                        if (index < 0 || index >= requestListModel.count) { clearEditor(); return }
                        var request = requestListModel.get(index)

                        reqIdField.text = request.id
                        reqClientIdField.text = request.clientId
                        reqProviderIdField.text = request.providerId
                        reqServiceIdField.text = request.serviceId
                        reqDescArea.text = request.description
                        reqCreatedAtField.text = request.createdAt
                        reqCompletedAtField.text = request.completedAt

                        var st = Number(request.status)
                        if (isNaN(st)) st = 0; if (st < 0) st = 0; if (st > 4) st = 4
                        reqStatusCombo.currentIndex = st

                        reqCommentField.text = ""
                        reqCommentsView.text = request.commentsText || ""
                    }

                    ListModel { id: requestListModel }

                    function reloadRequests() {
                        requestListModel.clear()
                        var list = dataManager.getAllRequests()
                        for (var i = 0; i < list.length; ++i) {
                            var r = list[i]
                            var comms = r.comments || []
                            var commText = ""
                            if (Array.isArray(comms)) {
                                for(var j=0; j<comms.length; j++) commText += "• " + comms[j] + "\n"
                            }
                            requestListModel.append({
                                id: r.id, serviceId: r.serviceId, clientId: r.clientId, providerId: r.providerId,
                                status: r.status, description: r.description, createdAt: r.createdAt, completedAt: r.completedAt,
                                commentsText: commText
                            })
                        }

                        // Пытаемся сохранить выбор или выбрать первый элемент
                        if (requestListModel.count > 0) {
                            if (currentRequestIndex < 0 || currentRequestIndex >= requestListModel.count)
                                currentRequestIndex = 0
                            requestListView.currentIndex = currentRequestIndex
                            loadRequestToEditor(currentRequestIndex)
                        } else {
                            clearEditor()
                        }
                    }

                    Connections {
                        target: dataManager
                        function onRequestsChanged() { requestPage.reloadRequests() }
                        function onLoggedInChanged() { requestPage.reloadRequests() }
                        Component.onCompleted: requestPage.reloadRequests()
                    }

                    ColumnLayout {
                        anchors.fill: parent; spacing: 12

                        RowLayout {
                            Layout.fillWidth: true; spacing: 12
                            Text { text: "Request Management"; color: "#61dafb"; font.pixelSize: 26; font.bold: true; Layout.fillWidth: true }

                            // КНОПКА ДОБАВЛЕНИЯ
                            Button {
                                text: "+ New"
                                Layout.preferredWidth: 80; Layout.preferredHeight: 40
                                background: Rectangle { color: "#4CAF50"; radius: 6 }
                                contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                onClicked: {
                                    var newId = dataManager.createRequest("", "", "New Manual Request")
                                    if (newId) {
                                        setInfo("Created: " + newId)
                                        requestPage.currentRequestIndex = 0 // Выбираем новую (первую) заявку
                                    }
                                }
                            }

                            PrimaryBtn { text: "Reload"; Layout.preferredWidth: 100; Layout.preferredHeight: 40; onClicked: requestPage.reloadRequests() }
                        }
                        Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: (uiInfo !== "" || uiError !== "") ? 44 : 0
                            visible: (uiInfo !== "" || uiError !== ""); radius: 8
                            color: (uiError !== "") ? "#3a1f1f" : "#1f2b22"; border.color: (uiError !== "") ? "#ff5252" : "#4CAF50"; border.width: 1
                            Text { anchors.centerIn: parent; text: uiError !== "" ? uiError : uiInfo; color: "white"; font.pixelSize: 12; font.bold: true }
                        }

                        RowLayout {
                            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 12

                            Card {
                                Layout.preferredWidth: 320; Layout.fillHeight: true; border.width: 2
                                ColumnLayout {
                                    anchors.fill: parent; anchors.margins: 10; spacing: 8
                                    Text { text: "Requests (" + requestListModel.count + ")"; color: "#888"; font.bold: true }
                                    ListView {
                                        id: requestListView; Layout.fillWidth: true; Layout.fillHeight: true; clip: true; model: requestListModel; spacing: 8
                                        currentIndex: requestPage.currentRequestIndex
                                        onCurrentIndexChanged: {
                                            requestPage.currentRequestIndex = currentIndex
                                            requestPage.loadRequestToEditor(currentIndex)
                                        }
                                        delegate: Rectangle {
                                            width: requestListView.width; height: 78; radius: 8
                                            color: (index === requestListView.currentIndex) ? "#2196F3" : "#1a1a1a"
                                            border.color: "#2196F3"; border.width: 1
                                            ColumnLayout {
                                                anchors.fill: parent; anchors.margins: 10; spacing: 4
                                                Text { text: description ? String(description).substring(0,40) : "No description"; color: "white"; font.bold: true; font.pixelSize: 12; elide: Text.ElideRight; Layout.fillWidth: true }
                                                Text { text: "ID: " + (id ? String(id).substring(0,8) : "???") + "..."; color: "#ccc"; font.pixelSize: 10 }
                                                RowLayout {
                                                    Layout.fillWidth: true
                                                    Text {
                                                        property var stNames: ["Pending", "Accepted", "In Progress", "Completed", "Cancelled"]
                                                        property var stColors: ["#FF9800", "#2196F3", "#00BCD4", "#4CAF50", "#F44336"]
                                                        property int s: Number(status)
                                                        text: (s >= 0 && s < 5) ? stNames[s] : "Unknown"; color: (s >= 0 && s < 5) ? stColors[s] : "white"; font.bold: true; font.pixelSize: 11
                                                    }
                                                    Item { Layout.fillWidth: true }
                                                    Text { text: createdAt ? String(createdAt).substring(0,10) : ""; color: "#888"; font.pixelSize: 10 }
                                                }
                                            }
                                            TapHandler { onTapped: requestListView.currentIndex = index }
                                        }
                                    }
                                    DangerBtn {
                                        text: "Delete Request"
                                        Layout.fillWidth: true; Layout.preferredHeight: 40
                                        enabled: reqIdField.text.trim().length > 0
                                        onClicked: {
                                            var rid = reqIdField.text.trim()
                                            var oldIdx = requestListView.currentIndex
                                            if (dataManager.deleteRequest(rid)) {
                                                setInfo("Request deleted")
                                                // Если удалили, пробуем выбрать предыдущий элемент
                                                if (oldIdx > 0) requestPage.currentRequestIndex = oldIdx - 1
                                                else requestPage.currentRequestIndex = 0
                                            } else setError("Failed to delete request")
                                        }
                                    }
                                }
                            }

                            Card {
                                Layout.fillWidth: true; Layout.fillHeight: true; border.width: 2
                                ScrollView {
                                    anchors.fill: parent; clip: true
                                    ColumnLayout {
                                        width: parent.width - 24; x: 12; y: 12; spacing: 12

                                        FieldLabel { text: "Request ID" }
                                        Field { id: reqIdField; Layout.fillWidth: true; readOnly: true; placeholderText: "Select request..." }

                                        RowLayout {
                                            Layout.fillWidth: true; spacing: 10
                                            ColumnLayout { Layout.fillWidth: true; FieldLabel { text: "Service ID" } Field { id: reqServiceIdField; Layout.fillWidth: true; readOnly: true } }
                                            ColumnLayout { Layout.fillWidth: true; FieldLabel { text: "Client ID" } Field { id: reqClientIdField; Layout.fillWidth: true; readOnly: true } }
                                            ColumnLayout { Layout.fillWidth: true; FieldLabel { text: "Provider ID" } Field { id: reqProviderIdField; Layout.fillWidth: true; readOnly: true } }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true; spacing: 10
                                            ColumnLayout {
                                                Layout.preferredWidth: 200; FieldLabel { text: "Status" }
                                                ComboBox {
                                                    id: reqStatusCombo; Layout.fillWidth: true; Layout.preferredHeight: 42
                                                    model: ["Pending", "Accepted", "In Progress", "Completed", "Cancelled"]
                                                    enabled: reqIdField.text.trim().length > 0
                                                    background: Rectangle { color: "#1a1a1a"; border.color: "#2196F3"; border.width: 1; radius: 6 }
                                                    contentItem: Text { text: reqStatusCombo.currentText; color: "white"; font.pixelSize: 12; verticalAlignment: Text.AlignVCenter; leftPadding: 10 }
                                                }
                                            }
                                            Button {
                                                Layout.preferredHeight: 42; Layout.preferredWidth: 100; text: "Update"; background: Rectangle { color: "#4CAF50"; radius: 6 }
                                                contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                                enabled: reqIdField.text.trim().length > 0
                                                onClicked: {
                                                    if (dataManager.updateRequestStatus(reqIdField.text.trim(), reqStatusCombo.currentIndex)) setInfo("Status updated")
                                                    else setError("Failed")
                                                }
                                            }
                                            Item { Layout.fillWidth: true }
                                            PrimaryBtn {
                                                text: "Open Chat 💬"; Layout.preferredWidth: 140; Layout.preferredHeight: 42
                                                enabled: reqIdField.text.trim().length > 0
                                                onClicked: { gotoPage(6); messagePage.loadChat(reqIdField.text.trim()) }
                                            }
                                        }

                                        FieldLabel { text: "Description" }
                                        TextArea {
                                            id: reqDescArea; Layout.fillWidth: true; Layout.preferredHeight: 60; color: "white"; wrapMode: TextArea.Wrap; placeholderText: "Description..."
                                            background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                        }
                                        Button {
                                            text: "Save Description"; Layout.preferredHeight: 36; Layout.fillWidth: true; background: Rectangle { color: "#333"; radius: 4 }
                                            contentItem: Text { text: parent.text; color: "#61dafb"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                            enabled: reqIdField.text.trim().length > 0
                                            onClicked: {
                                                if (dataManager.updateRequestDescription(reqIdField.text.trim(), reqDescArea.text)) setInfo("Description saved")
                                                else setError("Failed")
                                            }
                                        }

                                        FieldLabel { text: "Add Comment" }
                                        RowLayout {
                                            Layout.fillWidth: true; spacing: 10
                                            Field { id: reqCommentField; Layout.fillWidth: true; placeholderText: "Type a comment..." }
                                            Button {
                                                text: "Add"; Layout.preferredWidth: 80; Layout.preferredHeight: 42; background: Rectangle { color: "#333"; radius: 6 }
                                                contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                                enabled: reqIdField.text.trim().length > 0
                                                onClicked: {
                                                    if (reqCommentField.text.trim() === "") return
                                                    if (dataManager.addRequestComment(reqIdField.text.trim(), reqCommentField.text.trim())) { setInfo("Comment added"); reqCommentField.text = "" }
                                                    else setError("Failed")
                                                }
                                            }
                                        }

                                        FieldLabel { text: "Comments History" }
                                        TextArea {
                                            id: reqCommentsView; Layout.fillWidth: true; Layout.preferredHeight: 100; readOnly: true; color: "#ccc"; wrapMode: TextArea.Wrap
                                            background: Rectangle { color: "#1a1a1a"; radius: 6; border.color: "#444"; border.width: 1 }
                                        }

                                        RowLayout {
                                            Layout.fillWidth: true
                                            Field { id: reqCreatedAtField; Layout.fillWidth: true; readOnly: true; placeholderText: "Created At" }
                                            Field { id: reqCompletedAtField; Layout.fillWidth: true; readOnly: true; placeholderText: "Completed At" }
                                        }
                                        Item { Layout.fillHeight: true; Layout.preferredHeight: 20 }
                                    }
                                }
                            }
                        }
                    }
                }




                // PAGE 6: MESSENGER (CHAT)
                Item {
                    id: messagePage
                    property string currentRequestId: ""

                    // Функция загрузки чата (вызывай её при выборе заявки)
                    function loadChat(reqId) {
                        if (!reqId) return
                        currentRequestId = reqId
                        chatHeaderTitle.text = "Chat: " + reqId.substring(0, 8) + "..."
                        reloadMessages()
                    }

                    // Обновление сообщений
                    function reloadMessages() {
                        if (currentRequestId === "" || !messagePage.visible) return

                        // Берем все заявки и ищем ту, что открыта
                        var reqs = dataManager.getAllRequests()
                        messageModel.clear()

                        for (var i = 0; i < reqs.length; ++i) {
                            if (reqs[i].id === currentRequestId) {
                                var comments = reqs[i].comments || []
                                for (var j = 0; j < comments.length; ++j) {
                                    // В текущей реализации C++ все комментарии — это сообщения
                                    messageModel.append({
                                        content: comments[j],
                                        isMe: true, // Для простоты считаем все своими, либо добавь логику проверки автора
                                        time: ""
                                    })
                                }
                                break
                            }
                        }
                        msgListView.positionViewAtEnd()
                    }

                    // ТА САМАЯ ФУНКЦИЯ ОТПРАВКИ
                    function sendMessage() {
                        var txt = chatInput.text.trim()
                        if (txt === "" || currentRequestId === "") return

                        // ВАЖНО: Вызываем addRequestComment (этот метод 100% есть в твоем DataManager.cpp)
                        var ok = dataManager.addRequestComment(currentRequestId, txt)

                        if (ok) {
                            console.log("Message sent successfully")
                            chatInput.text = "" // Очищаем поле
                            reloadMessages()    // Мгновенно обновляем список
                        } else {
                            console.log("Error sending message")
                        }
                    }

                    ListModel { id: messageModel }

                    // Автообновление (чтобы видеть новые сообщения)
                    Timer {
                        interval: 2000; running: messagePage.visible && currentRequestId !== ""; repeat: true
                        onTriggered: messagePage.reloadMessages()
                    }

                    ColumnLayout {
                        anchors.fill: parent; spacing: 0

                        // HEADER
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 60; color: "#1f2b22"
                            Rectangle { anchors.bottom: parent.bottom; height: 1; color: "#333"; width: parent.width }
                            RowLayout {
                                anchors.fill: parent; anchors.margins: 10
                                Button {
                                    text: "← Back"
                                    onClicked: gotoPage(5)
                                    background: Rectangle { color: "transparent" }
                                    contentItem: Text { text: parent.text; color: "#61dafb"; font.bold: true }
                                }
                                Text {
                                    id: chatHeaderTitle
                                    text: "Select a chat"
                                    color: "white"; font.bold: true; font.pixelSize: 16
                                    Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter
                                }
                                Item { width: 50 }
                            }
                        }

                        // MESSAGES LIST
                        ListView {
                            id: msgListView
                            Layout.fillWidth: true; Layout.fillHeight: true; clip: true
                            model: messageModel; spacing: 10
                            leftMargin: 15; rightMargin: 15; topMargin: 15; bottomMargin: 15

                            delegate: ColumnLayout {
                                width: msgListView.width - 30
                                Layout.alignment: isMe ? Qt.AlignRight : Qt.AlignLeft

                                Rectangle {
                                    color: isMe ? "#2196F3" : "#333"
                                    radius: 12
                                    implicitWidth: Math.min(msgText.implicitWidth + 24, msgListView.width * 0.7)
                                    implicitHeight: msgText.implicitHeight + 20
                                    Text {
                                        id: msgText
                                        anchors.centerIn: parent; width: parent.width - 24
                                        text: content; color: "white"; wrapMode: Text.Wrap
                                    }
                                }
                            }
                        }

                        // INPUT AREA
                        Rectangle {
                            Layout.fillWidth: true; Layout.preferredHeight: 70; color: "#1a1a1a"
                            Rectangle { anchors.top: parent.top; height: 1; color: "#333"; width: parent.width }
                            RowLayout {
                                anchors.fill: parent; anchors.margins: 10; spacing: 10
                                TextField {
                                    id: chatInput
                                    Layout.fillWidth: true; Layout.preferredHeight: 45
                                    placeholderText: "Type a message..."
                                    color: "white"
                                    background: Rectangle { color: "#333"; radius: 22 }
                                    onAccepted: messagePage.sendMessage()
                                }
                                Button {
                                    text: "Send"
                                    Layout.preferredWidth: 80; Layout.preferredHeight: 45
                                    onClicked: messagePage.sendMessage()
                                    background: Rectangle { color: "#4CAF50"; radius: 22 }
                                    contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                                }
                            }
                        }
                    }
                }







                // PAGE 7: REVIEW (готовый, кнопка работает)
                Item {
                    id: reviewPage

                    property string uiInfo: ""
                    property string uiError: ""
                    property string currentServiceId: ""

                    function setInfo(msg) { uiInfo = msg; uiError = "" }
                    function setError(msg) { uiError = msg; uiInfo = "" }

                    // Локальное хранилище отзывов (в памяти QML)
                    QtObject {
                        id: reviewStore
                        property var _byService: ({}) // { serviceId: [ {authorId, rating, comment, createdAt}, ... ] }

                        function _ensure(serviceId) {
                            var sid = (serviceId || "").trim()
                            if (sid === "") return []
                            if (!_byService[sid])
                                _byService[sid] = []
                            return _byService[sid]
                        }

                        function list(serviceId) {
                            var arr = _ensure(serviceId)
                            // копия массива, чтобы случайно не мутировать
                            return arr.slice(0)
                        }

                        function add(serviceId, authorId, rating, comment) {
                            var sid = (serviceId || "").trim()
                            if (sid === "") return false

                            var c = (comment || "").trim()
                            if (c.length === 0) return false

                            var r = Number(rating)
                            if (isNaN(r)) r = 0
                            if (r < 1) r = 1
                            if (r > 5) r = 5

                            var a = (authorId || "").trim()
                            if (a === "") a = "anonymous"

                            var arr = _ensure(sid)
                            arr.unshift({
                                authorId: a,
                                rating: r,
                                comment: c,
                                createdAt: new Date().toISOString()
                            })

                            // опционально ограничим историю
                            if (arr.length > 200) arr.pop()
                            return true
                        }

                        function seedIfEmpty(serviceId) {
                            var arr = _ensure(serviceId)
                            if (arr.length > 0) return
                            arr.push({ authorId: "demo-user", rating: 5, comment: "Отличный сервис!", createdAt: "2026-01-10T10:00:00Z" })
                            arr.push({ authorId: "demo-user2", rating: 4, comment: "Хорошо, но можно быстрее.", createdAt: "2026-01-11T10:00:00Z" })
                        }
                    }

                    ListModel { id: reviewModel }

                    function loadReviews(serviceId) {
                        uiInfo = ""; uiError = ""
                        currentServiceId = (serviceId || "").trim()

                        reviewModel.clear()
                        if (currentServiceId === "") return

                        reviewStore.seedIfEmpty(currentServiceId)

                        var list = reviewStore.list(currentServiceId)
                        for (var i = 0; i < list.length; ++i) {
                            var r = list[i]
                            reviewModel.append({
                                authorId: r.authorId,
                                rating: r.rating,
                                comment: r.comment,
                                createdAt: r.createdAt
                            })
                        }
                    }

                    function ensureDefaultService() {
                        if (serviceModel.count <= 0) {
                            currentServiceId = ""
                            reviewModel.clear()
                            return
                        }

                        if (serviceSelector.currentIndex < 0 || serviceSelector.currentIndex >= serviceModel.count)
                            serviceSelector.currentIndex = 0

                        var sId = serviceModel.get(serviceSelector.currentIndex).sid
                        loadReviews(sId)
                    }

                    Component.onCompleted: ensureDefaultService()

                    Connections {
                        target: serviceModel
                        function onCountChanged() {
                            if (reviewPage.visible) ensureDefaultService()
                        }
                    }

                    onVisibleChanged: if (visible) ensureDefaultService()

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 15

                        // HEADER
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10

                            Text {
                                text: "Reviews & Ratings"
                                color: "#61dafb"
                                font.pixelSize: 24
                                font.bold: true
                            }
                            Item { Layout.fillWidth: true }

                            // Быстрый переход на логин, если не залогинен
                            Button {
                                visible: !dataManager.loggedIn
                                text: "Login"
                                Layout.preferredWidth: 120
                                Layout.preferredHeight: 36
                                background: Rectangle { color: "#333"; radius: 6; border.color: "#2196F3"; border.width: 1 }
                                contentItem: Text {
                                    text: parent.text
                                    color: "#61dafb"
                                    font.bold: true
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                                onClicked: gotoPage(1)
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                        // INFO/ERROR BAR
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: (uiInfo !== "" || uiError !== "") ? 44 : 0
                            visible: (uiInfo !== "" || uiError !== "")
                            radius: 8
                            color: (uiError !== "") ? "#3a1f1f" : "#1f2b22"
                            border.color: (uiError !== "") ? "#ff5252" : "#4CAF50"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: uiError !== "" ? uiError : uiInfo
                                color: "white"
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                            }
                        }

                        // SERVICE SELECTOR
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 60
                            color: "#2a2a2a"
                            radius: 5
                            border.color: "#444"
                            border.width: 1

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 10
                                spacing: 20

                                Text { text: "Select Service"; color: "#aaa" }

                                ComboBox {
                                    id: serviceSelector
                                    Layout.preferredWidth: 300
                                    textRole: "title"
                                    model: serviceModel

                                    onCurrentIndexChanged: {
                                        if (currentIndex >= 0 && currentIndex < serviceModel.count)
                                            reviewPage.loadReviews(serviceModel.get(currentIndex).sid)
                                    }
                                }

                                Item { Layout.fillWidth: true }

                                PrimaryBtn {
                                    text: "Reload"
                                    Layout.preferredWidth: 120
                                    Layout.preferredHeight: 40
                                    onClicked: {
                                        if (currentServiceId !== "")
                                            loadReviews(currentServiceId)
                                    }
                                }
                            }
                        }

                        // MAIN AREA
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 15

                            // LEFT: REVIEWS LIST
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                color: "#1e1e1e"
                                radius: 5
                                border.color: "#333"

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 10

                                    RowLayout {
                                        Layout.fillWidth: true
                                        Text { text: "Recent Reviews"; color: "white"; font.bold: true; font.pixelSize: 16 }
                                        Item { Layout.fillWidth: true }
                                        Text { text: reviewModel.count + ""; color: "#888"; font.pixelSize: 12 }
                                    }

                                    ListView {
                                        id: reviewsList
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        clip: true
                                        model: reviewModel
                                        spacing: 8

                                        delegate: Rectangle {
                                            width: reviewsList.width
                                            height: 76
                                            color: "#2a2a2a"
                                            radius: 6
                                            border.color: "#333"
                                            border.width: 1

                                            RowLayout {
                                                anchors.fill: parent
                                                anchors.margins: 10
                                                spacing: 10

                                                Rectangle {
                                                    width: 34
                                                    height: 34
                                                    radius: 17
                                                    color: "#444"
                                                    Text {
                                                        anchors.centerIn: parent
                                                        text: (authorId && authorId.length) ? authorId[0].toUpperCase() : "?"
                                                        color: "white"
                                                        font.bold: true
                                                    }
                                                }

                                                ColumnLayout {
                                                    Layout.fillWidth: true
                                                    spacing: 2

                                                    RowLayout {
                                                        Layout.fillWidth: true
                                                        Text { text: authorId; color: "#61dafb"; font.bold: true; font.pixelSize: 12 }
                                                        Item { Layout.fillWidth: true }
                                                        Text { text: "★".repeat(Math.round(rating)); color: "#FFD700"; font.pixelSize: 12 }
                                                    }

                                                    Text {
                                                        text: comment
                                                        color: "#ccc"
                                                        elide: Text.ElideRight
                                                        Layout.fillWidth: true
                                                    }

                                                    Text {
                                                        text: createdAt ? String(createdAt).replace("T"," ").replace("Z","") : ""
                                                        color: "#777"
                                                        font.pixelSize: 10
                                                        elide: Text.ElideRight
                                                        Layout.fillWidth: true
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            // RIGHT: ADD FORM
                            Rectangle {
                                Layout.preferredWidth: 320
                                Layout.fillHeight: true
                                color: "#1e1e1e"
                                radius: 5
                                border.color: "#333"

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.margins: 15
                                    spacing: 15

                                    Text { text: "Write Review"; color: "white"; font.bold: true; font.pixelSize: 16 }

                                    // STARS
                                    ColumnLayout {
                                        spacing: 5
                                        Text { text: "Rate"; color: "#aaa" }

                                        RowLayout {
                                            id: ratingRow
                                            spacing: 6
                                            property int selectedRating: 5

                                            Repeater {
                                                model: 5
                                                Button {
                                                    text: (index + 1).toString()
                                                    Layout.preferredWidth: 44
                                                    Layout.preferredHeight: 32

                                                    background: Rectangle {
                                                        radius: 4
                                                        color: (index + 1) <= ratingRow.selectedRating ? "#FFD700" : "#444"
                                                    }

                                                    contentItem: Text {
                                                        text: parent.text
                                                        color: (index + 1) <= ratingRow.selectedRating ? "black" : "white"
                                                        font.bold: true
                                                        horizontalAlignment: Text.AlignHCenter
                                                        verticalAlignment: Text.AlignVCenter
                                                    }

                                                    onClicked: ratingRow.selectedRating = index + 1
                                                }
                                            }
                                        }
                                    }

                                    // COMMENT
                                    ColumnLayout {
                                        Layout.fillHeight: true
                                        spacing: 6

                                        Text { text: "Comment"; color: "#aaa" }

                                        TextArea {
                                            id: reviewInput
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            color: "white"
                                            wrapMode: TextEdit.Wrap
                                            placeholderText: "Type review..."
                                            background: Rectangle { color: "#222"; border.color: "#444"; border.width: 1; radius: 6 }
                                        }
                                    }

                                    // SUBMIT BUTTON (РЕАЛИЗОВАНО)
                                    Button {
                                        id: submitBtn
                                        text: dataManager.loggedIn ? "Submit Review" : "Login to submit"
                                        Layout.fillWidth: true
                                        Layout.preferredHeight: 50
                                        enabled: true

                                        background: Rectangle {
                                            radius: 6
                                            color: submitBtn.down ? "#388E3C" : "#4CAF50"
                                            opacity: 1.0
                                        }

                                        contentItem: Text {
                                            text: parent.text
                                            font.bold: true
                                            color: "white"
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        onClicked: {
                                            reviewPage.setInfo("")
                                            reviewPage.setError("")

                                            if (!dataManager.loggedIn) {
                                                reviewPage.setError("Login required to submit review.")
                                                gotoPage(1)
                                                return
                                            }

                                            var sid = String(reviewPage.currentServiceId || "").trim()
                                            if (sid.length === 0) {
                                                reviewPage.setError("Select service first.")
                                                return
                                            }

                                            var comment = String(reviewInput.text || "").trim()
                                            if (comment.length === 0) {
                                                reviewPage.setError("Comment is empty.")
                                                return
                                            }

                                            var rating = Number(ratingRow.selectedRating)
                                            if (isNaN(rating)) rating = 5
                                            rating = Math.max(1, Math.min(5, Math.round(rating)))

                                            var author = String(dataManager.currentUserId || "anonymous")

                                            var ok = reviewStore.add(sid, author, rating, comment)
                                            if (!ok) {
                                                reviewPage.setError("Failed to add review.")
                                                return
                                            }

                                            reviewInput.text = ""
                                            reviewPage.loadReviews(sid)
                                            reviewPage.setInfo("Review submitted.")
                                        }
                                    }
                                }
                            }
                        }
                    }
                }











                // =======================
                // PAGE 8: SUBSCRIPTION (fixed click layer)
                // =======================
                Item {
                    id: subscriptionPage
                    anchors.fill: parent
                    enabled: true
                    visible: true
                    focus: true

                    property string uiInfo: ""
                    property string uiError: ""
                    property string currentSummary: "No subscription."

                    property int termDays: 30
                    property double basicPrice: 199
                    property double proPrice: 499

                    function setInfo(msg) { uiInfo = msg; uiError = "" }
                    function setError(msg) { uiError = msg; uiInfo = "" }
                    function t(x) { return (x === undefined || x === null) ? "" : String(x).trim() }

                    function isoToday() { return new Date().toISOString().slice(0, 10) }
                    function isoAddDays(days) {
                        var d = new Date()
                        d.setDate(d.getDate() + Number(days))
                        return d.toISOString().slice(0, 10)
                    }

                    function parseIsoDateOnly(iso10) { return new Date(String(iso10) + "T00:00:00Z") }
                    function daysBetweenIso(startIso10, endIso10) {
                        var a = parseIsoDateOnly(startIso10)
                        var b = parseIsoDateOnly(endIso10)
                        var ms = b.getTime() - a.getTime()
                        if (isNaN(ms)) return 0
                        return Math.floor(ms / 86400000) + 1
                    }

                    function selectedPlan() { return planCombo.currentText }
                    function selectedPrice() { return (selectedPlan() === "Pro") ? proPrice : basicPrice }

                    function buildSummary(plan, startIso10, endIso10, active) {
                        var days = daysBetweenIso(startIso10, endIso10)
                        return "Plan: " + plan +
                               " | Period: " + startIso10 + " — " + endIso10 + " (" + days + " days)" +
                               " | " + (active ? "Active" : "Inactive")
                    }

                    function loadMySubscriptionToUi() {
                        uiInfo = ""; uiError = ""

                        if (!dataManager.loggedIn) {
                            currentSummary = "Please login to subscribe."
                            setError("Login required")
                            return
                        }

                        var s = dataManager.getMySubscription()
                        if (!s || Object.keys(s).length === 0) {
                            currentSummary = "No subscription. Choose a plan and click SUBSCRIBE."
                            setInfo("No subscription yet.")
                            return
                        }

                        var plan = t(s.planType)
                        var startIso10 = t(s.startDate).slice(0, 10)
                        var endIso10 = t(s.endDate).slice(0, 10)
                        var active = !!s.active

                        currentSummary = buildSummary(plan, startIso10, endIso10, active)
                        setInfo("Subscription loaded.")
                    }

                    function purchase() {
                        setInfo("purchase(): start")
                        if (!dataManager.loggedIn) { setError("Login required"); return }

                        var plan = selectedPlan()
                        var price = selectedPrice()
                        var startIso10 = isoToday()
                        var endIso10 = isoAddDays(termDays)

                        var ok = dataManager.saveMySubscription(plan, price, startIso10, endIso10, true)
                        if (ok) { setInfo("purchase(): OK"); loadMySubscriptionToUi() }
                        else { setError("purchase(): FAILED") }
                    }

                    function cancelSub() {
                        setInfo("cancelSub(): start")
                        if (!dataManager.loggedIn) { setError("Login required"); return }

                        var ok = dataManager.cancelMySubscription()
                        if (ok) { setInfo("cancelSub(): OK"); loadMySubscriptionToUi() }
                        else { setError("cancelSub(): FAILED (maybe no subscription)") }
                    }

                    Connections {
                        target: dataManager
                        function onSubscriptionsChanged() { if (subscriptionPage.visible) subscriptionPage.loadMySubscriptionToUi() }
                        function onLoggedInChanged() { if (subscriptionPage.visible) subscriptionPage.loadMySubscriptionToUi() }
                        function onCurrentUserChanged() { if (subscriptionPage.visible) subscriptionPage.loadMySubscriptionToUi() }
                    }

                    onVisibleChanged: if (visible) loadMySubscriptionToUi()
                    Component.onCompleted: loadMySubscriptionToUi()

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        Text { text: "Subscription"; color: "#61dafb"; font.pixelSize: 26; font.bold: true }
                        Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                        // status bar
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 44
                            radius: 8
                            color: (uiError !== "") ? "#3a1f1f" : "#1f2b22"
                            border.color: (uiError !== "") ? "#ff5252" : "#4CAF50"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: (uiError !== "") ? uiError : ((uiInfo !== "") ? uiInfo : "Ready")
                                color: "white"
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                            }
                        }

                        // purchase card (поднимаем слой, чтобы точно не перекрывалось чем-то ниже)
                        CardPane {
                            Layout.fillWidth: true
                            z: 50

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                FieldLabel { text: "Plan" }

                                ComboBox {
                                    id: planCombo
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 42
                                    model: ["Basic", "Pro"]

                                    background: Rectangle {
                                        color: "#1a1a1a"
                                        border.color: "#2196F3"
                                        border.width: 1
                                        radius: 6
                                    }
                                    contentItem: Text {
                                        text: planCombo.currentText
                                        color: "white"
                                        font.pixelSize: 12
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 10
                                        elide: Text.ElideRight
                                    }

                                    onCurrentIndexChanged: {
                                        priceText.text = "Price: " + Number(selectedPrice()).toFixed(0) +
                                                         " | Term: " + termDays + " days"
                                    }
                                }

                                Text {
                                    id: priceText
                                    text: "Price: " + Number(selectedPrice()).toFixed(0) + " | Term: " + termDays + " days"
                                    color: "#9aa7b5"
                                    font.pixelSize: 12
                                }

                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 10

                                    Button {
                                        text: "SUBSCRIBE"
                                        Layout.preferredWidth: 160
                                        Layout.preferredHeight: 44
                                        z: 100

                                        background: Rectangle {
                                            radius: 6
                                            color: parent.down ? "#1976D2" : "#2196F3"
                                            border.color: "#61dafb"
                                            border.width: 1
                                        }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "white"
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        onClicked: {
                                            setInfo("SUBSCRIBE clicked")
                                            if (!dataManager.loggedIn) { setError("Login required"); gotoPage(1); return }
                                            purchase()
                                        }
                                    }

                                    Button {
                                        text: "CANCEL"
                                        Layout.preferredWidth: 160
                                        Layout.preferredHeight: 44
                                        z: 100

                                        background: Rectangle {
                                            radius: 6
                                            color: parent.down ? "#B71C1C" : "#d32f2f"
                                            border.color: "#ff5252"
                                            border.width: 1
                                        }
                                        contentItem: Text {
                                            text: parent.text
                                            color: "white"
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                        }

                                        onClicked: {
                                            setInfo("CANCEL clicked")
                                            if (!dataManager.loggedIn) { setError("Login required"); gotoPage(1); return }
                                            cancelSub()
                                        }
                                    }
                                }
                            }
                        }

                        // current subscription info
                        CardPane {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            z: 1

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 8

                                Text { text: "Current subscription"; color: "#61dafb"; font.pixelSize: 14; font.bold: true }

                                Text {
                                    Layout.fillWidth: true
                                    text: currentSummary
                                    color: "white"
                                    font.pixelSize: 12
                                    wrapMode: Text.WordWrap
                                }

                                Item { Layout.fillHeight: true }
                            }
                        }
                    }
                }










                // =======================
                // PAGE 9: FAVORITES (MINIMAL LIST)
                // =======================
                Item {
                    id: favoritesPage

                    property string uiInfo: ""
                    property string uiError: ""
                    function setInfo(m) { uiInfo = m; uiError = "" }
                    function setError(m) { uiError = m; uiInfo = "" }
                    function t(x) { return (x === undefined || x === null) ? "" : String(x).trim() }

                    ListModel { id: favListModel } // { sid, title, category, price }

                    function findServiceInModel(sid) {
                        var id = t(sid)
                        for (var i = 0; i < serviceModel.count; ++i) {
                            var s = serviceModel.get(i)
                            if (t(s.sid) === id) return s
                        }
                        return null
                    }

                    function reloadFavorites() {
                        uiInfo = ""; uiError = ""
                        favListModel.clear()

                        if (!dataManager.loggedIn) { setError("Login required"); return }

                        var f = dataManager.getMyFavorites()
                        var ids = (f && f.favoriteServiceIds) ? f.favoriteServiceIds : []

                        for (var i = 0; i < ids.length; ++i) {
                            var sid = t(ids[i])
                            var s = findServiceInModel(sid)

                            favListModel.append({
                                sid: sid,
                                title: s ? (s.title || "Untitled") : "Unknown service",
                                category: s ? (s.category || "") : "",
                                price: s ? (s.price || 0) : 0
                            })
                        }

                        setInfo("Favorites: " + favListModel.count)
                    }

                    Connections {
                        target: dataManager
                        function onFavoritesChanged() { if (favoritesPage.visible) favoritesPage.reloadFavorites() }
                        function onLoggedInChanged() { if (favoritesPage.visible) favoritesPage.reloadFavorites() }
                    }

                    onVisibleChanged: if (visible) reloadFavorites()

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true
                            Text { text: "Favorites"; color: "#61dafb"; font.pixelSize: 26; font.bold: true }
                            Item { Layout.fillWidth: true }
                            PrimaryBtn {
                                text: "Reload"
                                Layout.preferredWidth: 120
                                Layout.preferredHeight: 40
                                enabled: dataManager.loggedIn
                                onClicked: reloadFavorites()
                            }
                        }

                        Rectangle { Layout.fillWidth: true; height: 1; color: "#2196F3" }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            radius: 8
                            color: (uiError !== "") ? "#3a1f1f" : "#1f2b22"
                            border.color: (uiError !== "") ? "#ff5252" : "#4CAF50"
                            border.width: 1
                            Text {
                                anchors.centerIn: parent
                                text: (uiError !== "") ? uiError : (uiInfo !== "" ? uiInfo : "Ready")
                                color: "white"
                                font.pixelSize: 12
                                font.bold: true
                                elide: Text.ElideRight
                            }
                        }

                        Card {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            border.width: 2

                            ListView {
                                anchors.fill: parent
                                anchors.margins: 12
                                clip: true
                                model: favListModel
                                spacing: 8

                                delegate: Rectangle {
                                    width: parent.width
                                    height: 72
                                    radius: 10
                                    color: "#1a1a1a"
                                    border.color: "#2196F3"
                                    border.width: 1

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        spacing: 10

                                        ColumnLayout {
                                            Layout.fillWidth: true
                                            spacing: 2

                                            Text {
                                                text: title
                                                color: "white"
                                                font.bold: true
                                                font.pixelSize: 14
                                                elide: Text.ElideRight
                                            }
                                            Text {
                                                text: category + "  " + Number(price).toFixed(0) + "  |  " + sid
                                                color: "#9aa7b5"
                                                font.pixelSize: 11
                                                elide: Text.ElideRight
                                            }
                                        }

                                        // по желанию: быстро убрать из избранного
                                        Button {
                                            text: "★"
                                            Layout.preferredWidth: 44
                                            Layout.preferredHeight: 32
                                            background: Rectangle { radius: 6; color: "#FFD700" }
                                            contentItem: Text {
                                                text: parent.text
                                                color: "black"
                                                font.bold: true
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                            onClicked: {
                                                if (!dataManager.loggedIn) return
                                                dataManager.toggleFavoriteService(sid) // уберёт (toggle) [file:21][file:19]
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }
}

