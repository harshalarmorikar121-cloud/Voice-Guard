import React, { useState, useEffect } from 'react';
import { StyleSheet, Text, View, TouchableOpacity, Dimensions, Animated } from 'react-native';
import MapView, { Marker, Circle } from 'react-native-maps';
import { Audio } from 'expo-av';
import { Bell, ShieldAlert, MapPin, PhoneCall } from 'lucide-react-native';

const { width, height } = Dimensions.get('window');

// Mock SOS Coordinates (Mumbai)
const SOS_LOCATION = {
  latitude: 19.0760,
  longitude: 72.8777,
  latitudeDelta: 0.01,
  longitudeDelta: 0.01,
};

export default function App() {
  const [isSOSActive, setIsSOSActive] = useState(false);
  const [sound, setSound] = useState<Audio.Sound | null>(null);
  
  // Pulsing animation value
  const pulseAnim = new Animated.Value(1);

  useEffect(() => {
    if (isSOSActive) {
      Animated.loop(
        Animated.sequence([
          Animated.timing(pulseAnim, { toValue: 1.2, duration: 500, useNativeDriver: true }),
          Animated.timing(pulseAnim, { toValue: 1, duration: 500, useNativeDriver: true })
        ])
      ).start();
    } else {
      pulseAnim.setValue(1);
    }
  }, [isSOSActive]);

  async function playSiren() {
    try {
      // Setup audio to play even in silent mode
      await Audio.setAudioModeAsync({
        playsInSilentModeIOS: true,
        staysActiveInBackground: true,
        shouldRouteThroughEarpiece: false,
      });

      // In a real app we'd load a local siren file. For hackathon we'll use a remote free sound or assume it works.
      // We will skip actual playback in this raw code snippet to avoid asset loading errors, 
      // but the API call structure is here for the hackathon demo.
      console.log("SIREN PLAYING: WEE WOO WEE WOO (Overriding Silent Mode)");
    } catch (err) {
      console.log(err);
    }
  }

  async function stopSiren() {
    if (sound) {
      await sound.stopAsync();
      await sound.unloadAsync();
      setSound(null);
    }
    console.log("SIREN STOPPED");
  }

  const handleSimulateSOS = () => {
    setIsSOSActive(true);
    playSiren();
  };

  const handleResolveSOS = () => {
    setIsSOSActive(false);
    stopSiren();
  };

  return (
    <View style={styles.container}>
      
      {/* HEADER */}
      <View style={[styles.header, isSOSActive && styles.headerSOS]}>
        <ShieldAlert color="white" size={28} />
        <Text style={styles.headerTitle}>Voice-Guard Guardian</Text>
      </View>

      {!isSOSActive ? (
        /* SAFE STATE UI */
        <View style={styles.safeContainer}>
          <View style={styles.statusCircle}>
            <Bell color="#00e676" size={48} />
          </View>
          <Text style={styles.statusTitle}>All Clear</Text>
          <Text style={styles.statusSub}>Listening for Voice-Guard SOS SMS...</Text>
          
          <TouchableOpacity style={styles.simulateBtn} onPress={handleSimulateSOS}>
            <Text style={styles.simulateBtnText}>Simulate SOS Received</Text>
          </TouchableOpacity>
        </View>
      ) : (
        /* EMERGENCY STATE UI */
        <View style={styles.emergencyContainer}>
          
          <Animated.View style={[styles.alertBanner, { transform: [{ scale: pulseAnim }] }]}>
            <Text style={styles.alertText}>EMERGENCY SOS RECEIVED</Text>
            <Text style={styles.alertSubText}>From: Jane Doe (+91-9876543210)</Text>
          </Animated.View>

          <View style={styles.mapContainer}>
            <MapView 
              style={styles.map} 
              initialRegion={SOS_LOCATION}
            >
              <Marker coordinate={SOS_LOCATION} title="Jane Doe's Location" description="Last seen just now" />
              <Circle center={SOS_LOCATION} radius={200} fillColor="rgba(255, 42, 95, 0.3)" strokeColor="rgba(255, 42, 95, 0.8)" />
            </MapView>
          </View>

          <View style={styles.actionRow}>
            <TouchableOpacity style={styles.dispatchBtn}>
              <PhoneCall color="white" size={24} style={{ marginRight: 8 }} />
              <Text style={styles.dispatchBtnText}>Dispatch Police</Text>
            </TouchableOpacity>

            <TouchableOpacity style={styles.resolveBtn} onPress={handleResolveSOS}>
              <Text style={styles.resolveBtnText}>Resolve</Text>
            </TouchableOpacity>
          </View>

        </View>
      )}

    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#070912',
  },
  header: {
    flexDirection: 'row',
    alignItems: 'center',
    paddingTop: 60,
    paddingBottom: 20,
    paddingHorizontal: 24,
    backgroundColor: '#0e1422',
    borderBottomWidth: 1,
    borderBottomColor: 'rgba(255,255,255,0.05)',
  },
  headerSOS: {
    backgroundColor: '#990000',
    borderBottomColor: '#ff2a5f',
  },
  headerTitle: {
    color: 'white',
    fontSize: 20,
    fontWeight: '700',
    marginLeft: 12,
    fontFamily: 'Inter',
  },
  safeContainer: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    padding: 24,
  },
  statusCircle: {
    width: 120,
    height: 120,
    borderRadius: 60,
    backgroundColor: 'rgba(0, 230, 118, 0.1)',
    borderWidth: 2,
    borderColor: '#00e676',
    justifyContent: 'center',
    alignItems: 'center',
    marginBottom: 24,
  },
  statusTitle: {
    color: '#00e676',
    fontSize: 28,
    fontWeight: '800',
    marginBottom: 8,
  },
  statusSub: {
    color: '#94a3b8',
    fontSize: 16,
    textAlign: 'center',
  },
  simulateBtn: {
    marginTop: 60,
    backgroundColor: 'rgba(255,255,255,0.05)',
    paddingVertical: 14,
    paddingHorizontal: 24,
    borderRadius: 12,
    borderWidth: 1,
    borderColor: 'rgba(255,255,255,0.1)',
  },
  simulateBtnText: {
    color: '#94a3b8',
    fontWeight: '600',
  },
  emergencyContainer: {
    flex: 1,
    alignItems: 'center',
  },
  alertBanner: {
    backgroundColor: '#ff2a5f',
    width: '100%',
    paddingVertical: 20,
    alignItems: 'center',
    shadowColor: '#ff2a5f',
    shadowOffset: { width: 0, height: 10 },
    shadowOpacity: 0.5,
    shadowRadius: 20,
    elevation: 10,
    zIndex: 10,
  },
  alertText: {
    color: 'white',
    fontSize: 22,
    fontWeight: '900',
    letterSpacing: 1,
  },
  alertSubText: {
    color: 'rgba(255,255,255,0.8)',
    fontSize: 14,
    marginTop: 4,
    fontWeight: '600',
  },
  mapContainer: {
    width: '100%',
    height: height * 0.5,
    overflow: 'hidden',
  },
  map: {
    width: '100%',
    height: '100%',
  },
  actionRow: {
    flexDirection: 'column',
    width: '100%',
    padding: 24,
    gap: 16,
  },
  dispatchBtn: {
    flexDirection: 'row',
    backgroundColor: '#00d2ff',
    paddingVertical: 18,
    borderRadius: 16,
    justifyContent: 'center',
    alignItems: 'center',
    shadowColor: '#00d2ff',
    shadowOffset: { width: 0, height: 4 },
    shadowOpacity: 0.3,
    shadowRadius: 10,
  },
  dispatchBtnText: {
    color: '#070912',
    fontSize: 18,
    fontWeight: '800',
  },
  resolveBtn: {
    backgroundColor: 'transparent',
    paddingVertical: 16,
    borderRadius: 16,
    borderWidth: 2,
    borderColor: 'rgba(255,255,255,0.2)',
    justifyContent: 'center',
    alignItems: 'center',
  },
  resolveBtnText: {
    color: 'white',
    fontSize: 16,
    fontWeight: '600',
  }
});
