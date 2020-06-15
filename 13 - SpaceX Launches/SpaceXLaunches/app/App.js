import React from "react";

// navigation
import { NavigationContainer } from "@react-navigation/native";
import { createStackNavigator } from "@react-navigation/stack";

// screens
import { HomeScreen } from "./pages/home";
import { LaunchScreen } from "./pages/launch";
import { LaunchListScreen } from "./pages/launchlist";

const Stack = createStackNavigator();
export default function App() {
    // initialize data variables
    global.dataLoaded = false;
    global.upcomingLaunches = [];
    global.pastLaunches = [];
    global.launchNames = [];
    global.nameData = {};

    return (
        <NavigationContainer>
            <Stack.Navigator
                initialRouteName="home"
                screenOptions={{
                    headerStyle: {
                        backgroundColor: "#f4511e",
                        height: 56
                    },
                    headerTintColor: "#fff",
                    headerTitleStyle: {
                        fontWeight: "bold"
                    }
                }}
            >
                <Stack.Screen
                    name="home"
                    component={HomeScreen}
                    options={{
                        headerStyle: {
                            backgroundColor: "#000846"
                        },
                        title: "SpaceX Launches"
                    }}
                />
                <Stack.Screen
                    name="launch"
                    component={LaunchScreen}
                    options={({ route }) => ({ title: route.params.name })}
                />
                <Stack.Screen
                    name="launchlist"
                    component={LaunchListScreen}
                    options={({ route }) => ({ title: route.params.name })}
                />
            </Stack.Navigator>
        </NavigationContainer>
    );
}