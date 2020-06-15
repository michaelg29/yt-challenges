import { StyleSheet } from "react-native";

export const styles = StyleSheet.create({
    container: {
        flex: 1,
        flexDirection: "column",
        justifyContent: "space-around"
    },
    row: {
        flexDirection: "row",
        justifyContent: "space-around"
    },
    button: {
        width: "50%",
        height: 46,
        margin: 4,
        flex: 1,
        flexDirection: "column",
        justifyContent: "center"
    },
    textImportant: {
        fontWeight: "bold",
        fontSize: 36
    },
    textCenter: {
        textAlign: "center"
    },
    listItem: {
        width: "100%",
        height: 56,
        flexDirection: "row",
        padding: 5,
        margin: 5
    },
    imageIcon: {
        margin: 5,
        height: 46,
        width: 46,
        resizeMode: "stretch"
    },
    listItemIcon: {
        width: 56,
        height: 56,
        justifyContent: "center"
    },
    listItemText: {
        height: 56,
        flexDirection: "column",
        justifyContent: "center",
        marginLeft: 15
    }
});